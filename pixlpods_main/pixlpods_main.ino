#include <Adafruit_NeoPixel.h>

#include "Colors.h"

#define NUM_PIXELS 10
#define PXL_PIN 6

#define INPUT_PIN 2
#define ADJUSTMENT_INTERVAL 50 //ms
#define DEBOUNCE_INTERVAL 50
#define OFF_LENGTH 3000 //ms
#define MODE_LENGTH 1000 //ms

#define INITIAL_COLOR BLUE

//
// SETTINGS
//

// Blink Mode
const int BLINK_SPEED = 5;  // 1 - 10

// Chase Mode
const int CHASE_SPEED = 5;  // 1 - 10
const int CHASE_LENGTH = 4; // Number of LEDs on (Max 7)

// Scan Mode
const int SCAN_SPEED = 5;  // 1 - ~10
const int TAIL_SIZE = 3;


enum MODES {
  SOLID,
  SOLID_BLINK,
  CHASE,
  SCAN,
  RAINBOW,
  HOLIDAY,
  RANDOM_BLINK,
  AURA,
  SAFETY
};

boolean leds_on         = false;
boolean adjustment_mode = false;
boolean scan_forward    = true;

uint8_t current_mode  = SOLID;
uint8_t current_speed = 5;

uint32_t chosen_color     = INITIAL_COLOR;
uint32_t chosen_palette   = 3;
uint32_t chosen_holiday   = 0;
uint32_t chosen_aura      = 1;
 
unsigned long counter = 0;

uint8_t last_reading        = HIGH;
uint8_t last_button_state   = HIGH;
uint8_t button_state;

unsigned long off_start = 0;
unsigned long adj_start = 0;
unsigned long debounce_start = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PXL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel onboard = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);

void setup() {  
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
  onboard.begin();
  onboard.setBrightness(50);
  onboard.show();
  pinMode(INPUT_PIN, INPUT_PULLUP);
}

void loop() {  
  if (leds_on) {
    if (adjustment_mode) {
      enter_adjustment_mode();
    } else {
      run_mode();
    }
  } 
  checkButton();
  counter++;
}

/*
 *  MODES
 */

void run_mode(){
  switch (current_mode) {
    case SOLID:
      setColor(chosen_color);
      break;
    case SOLID_BLINK:
      solidBlink(chosen_color);
      break;
    case CHASE:
      chase(chosen_color);
      break;
    case SCAN:
      scan(chosen_color);
      break;
    case RAINBOW:
      rainbowCycle();
      break;
    case HOLIDAY:
      holiday();
      break;
    case RANDOM_BLINK:
      random_blink(chosen_palette);
      break;
    case AURA:
      aura();
      break;
    case SAFETY:
      safety();
      break;
    default:
      break;
  };
}

void solidBlink(uint32_t color) {
  setColor(color);
  delay_and_check(2500 / BLINK_SPEED);
  clearColor();
  delay_and_check(2500 / BLINK_SPEED);
}

// CHASE
void chase(uint32_t color) {
 for (uint16_t i=0; i < (strip.numPixels() * 2); i++) {
    uint32_t c = 0;
    if(((counter + i) & 7) < CHASE_LENGTH) c = color; // 4 pixels on...
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay_and_check(500 / CHASE_SPEED); 
}

// SCAN
void scan(uint32_t color) {
  uint16_t n = strip.numPixels();
  uint16_t front;

  if (counter % n == 0) {
    scan_forward = !scan_forward;
  }

  front = scan_forward ? counter % n : n - (counter % n) - 1;

  uint32_t r, g, b;
  r = (color & 0xFF0000) >> 16;
  g = (color & 0x00FF00) >> 8;
  b = (color & 0x0000FF);
  
  for (uint16_t i = 0; i < n; i++) {
    boolean on = scan_forward ? (front - i) < TAIL_SIZE : (i - front) < TAIL_SIZE;
    if (on) {
      uint32_t r_i, g_i, b_i;
      uint32_t divisor = scan_forward ? (front - i + 1) * 3 : (i - front + 1) * 3;
      r_i = r / divisor;
      g_i = g / divisor;
      b_i = b / divisor;
      strip.setPixelColor(i, r_i, g_i, b_i);
    } else {
      strip.setPixelColor(i, 0);
    }
  }
  strip.show();
  delay_and_check(500 / SCAN_SPEED); 
}

// RAINBOW
void rainbowCycle() {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + (counter % 256*5)) & 255));
  }
  strip.show();
  delay_and_check((10 - current_speed) * 10);
}

// RANDOM BLINK
void random_blink(uint32_t chosen_palette){
  uint32_t colors[3] = {
    WHEEL[chosen_palette],
    WHEEL[(chosen_palette + 1) % 12],
    WHEEL[(chosen_palette + 2) % 12]
  };
  strip.setPixelColor(random(strip.numPixels()), colors[random(3)]);
  strip.show();
  delay_and_check(random(100,250));
}

// HOLIDAY
void holiday(){
  struct palette *holiday = &HOLIDAYS[chosen_holiday];
  uint8_t n = holiday->num_colors;
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, holiday->colors[i % n]);
  }
  strip.show();
  delay_and_check(5000);
}

// AURA

void aura(uint32_t* palette){
  uint8_t n = sizeof(palette) / sizeof(uint32_t);

  for(uint16_t i=0; i < strip.numPixels(); i++) {
    uint32_t color = palette[random(n)];
    uint8_t intensity_divisor = random(3); 
    uint32_t r, g, b;
    r = (color & 0xFF0000) >> 16;
    g = (color & 0x00FF00) >> 8;
    b = (color & 0x0000FF);
    
    strip.setPixelColor(i, r / intensity_divisor, g / intensity_divisor, b / intensity_divisor);
  }
  strip.show();
  delay_and_check(5);
}

void aura() {
  switch (chosen_aura) {
    case 1:
      fireAura();
      break;
    case 2:
      waterAura();
      break;
    case 3:
      earthAura();
      break;
    default:
      break;
  }
}

void waterAura() {
  for(uint16_t i=0; i< strip.numPixels(); i++) {
    uint8_t wave2 = 0x7F * sin((((counter + 10*i) % 360)*3.14159)/180) + 0x7F;
    uint8_t g_value = wave2;
    strip.setPixelColor(i, 0, g_value, 0xFF);
  }
  strip.show();
  delay_and_check(5);
}

void fireAura() {
  for(uint16_t i=0; i< strip.numPixels(); i++) {
    uint8_t wave2 = 0x0F * sin((2.5*((counter + 10*i) % 360)*3.14159)/180) + 0x0F;
    uint8_t g_value = wave2;
    if (random(500) == 0){
      strip.setPixelColor(i, YELLOW);
    } else {
      strip.setPixelColor(i, 0xFF, g_value, 0); 
    }
  }
  strip.show();
  delay_and_check(10);
}

void earthAura() {
  for(uint16_t i=0; i< strip.numPixels(); i++) {
    uint16_t x = random(1000);
    if (x < 350) {
      strip.setPixelColor(i, earth_palette[0]);
    } else if (x >= 350 && x < 750) {
      strip.setPixelColor(i, earth_palette[1]);
    } else if (x >= 750 && x < 950) {
      strip.setPixelColor(i, earth_palette[2]); 
    } else {
      strip.setPixelColor(i, earth_palette[3]);
    }
  }
  strip.show();
  delay_and_check(1000);
}

void fireAura2() {
  int r = 255;
  int g = r-40;
  int b = 40;
  
  for (uint16_t x = 0; x < strip.numPixels(); x++) {
    int flicker = random(0,100);
    int r1 = r;
    int g1 = g-flicker;
    int b1 = b-flicker;
    if(g1<0) g1=0;
    if(r1<0) r1=0;
    if(b1<0) b1=0;
    strip.setPixelColor(x, r1, g1, b1);
  }
  strip.show();
  delay_and_check(random(100,250));
}

void safety(){
  setColor(WHITE);
  delay_and_check(2500 / current_speed);
  setColor(RED);
  delay_and_check(2500 / current_speed);
}


float deg_to_rad(uint16_t deg) {
  return (float) deg * 3.14159 / 180;
}

/*
 *  COLOR UTILS
 */

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

// Set the strip to a color
void setColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

void clearColor() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
  }
  strip.show();
}

void colorSweep() {
  uint8_t  n = 0;
  uint32_t color = 0;
  while(adjustment_mode) {
    for(uint16_t i=0; i< strip.numPixels(); i++) {
      color = Wheel(n);
      strip.setPixelColor(i, color);
    }
    strip.show();
    n++;
    delay_and_check(ADJUSTMENT_INTERVAL);
    checkButton();
  }
  chosen_color = color;
}

void rotatePalettes() {
  uint32_t current_palette = chosen_palette;
  while (adjustment_mode) {
    uint32_t colors[3] = {
      WHEEL[current_palette],
      WHEEL[(current_palette + 1) % 12],
      WHEEL[(current_palette + 2) % 12]
    };
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, colors[i%3]);     
    }
    strip.show();
    current_palette++;
    if (current_palette > 12) {
      current_palette = 0;
    }
    if(delay_and_check(1000)){
      break;
    }
  }
  chosen_palette = current_palette;
}

void chooseHoliday() {
  uint8_t holiday_index = 0;
  while (adjustment_mode) {
    onboard.setPixelColor(0, BLUE);
    onboard.show();
    
    holiday_index++;
    if (holiday_index > 3) {
      holiday_index = 0; 
    } 
    
    struct palette *current_holiday = &HOLIDAYS[holiday_index];
    uint8_t n = current_holiday->num_colors;
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, current_holiday->colors[i % n]);     
    }
    strip.show();
    chosen_holiday = holiday_index;
    delay_and_check(1500);
  }
  onboard.setPixelColor(0, YELLOW);
  onboard.show();
}

void chooseAura() {
  while (adjustment_mode) {
    chosen_aura = chosen_aura == 3 ? 1 : chosen_aura + 1;
    unsigned long mode_start = millis();
    // Run mode for 500ms at current speed
    while ((mode_start + 2000) > millis()) {
      aura();
      counter++;
    }
  }
}

void adjustSpeed(){
  boolean reverse = false;
  current_speed = 2;
  while (adjustment_mode) {
    if (current_speed == 10 || current_speed == 1){
      reverse = !reverse;
    }
    if (reverse) {
      current_speed++;
    } else {
      current_speed--;
    }
    //current_speed = reverse ? current_speed-- : current_speed++;
    unsigned long mode_start = millis();
    // Run mode for 1 sec at current speed
    while ((mode_start + 1000) > millis()){
      run_mode();
      counter++;
    }
  }
}

/*
 *  CONTROL
 */

boolean checkButton() {
  boolean button_pressed = false;
  int reading = digitalRead(INPUT_PIN);
  
  if (reading != last_reading) {
    debounce_start = millis(); 
  }
  
  if ((millis() - debounce_start) > DEBOUNCE_INTERVAL) {
    if (reading != button_state) {
      button_state = reading;
      if (button_state == LOW){
        adj_start = millis();
        off_start = millis();
      }
      if (button_state == HIGH && last_button_state == LOW) {
        if ((millis() - adj_start) <= MODE_LENGTH) {
          if (leds_on) {
            if (adjustment_mode) {
              adjustment_mode = false;
            } else {
              current_mode = current_mode > AURA ? 0 : current_mode + 1;
              clearColor();   
            }
          } else {
            leds_on = true;
          }
        }
        button_pressed = true; 
      }
    }
    if (button_state == LOW && 
        adj_start > 0 && ((millis() - adj_start) > MODE_LENGTH)){
     adj_start = 0;
     adjustment_mode = true;
   }
    
   if (button_state == LOW && 
       off_start > 0 && ((millis() - off_start) > OFF_LENGTH)){
     off_start = 0;
     adjustment_mode = false;
     off();
    }
    last_button_state = button_state;    
  }
  
  if (button_state == LOW){
    onboard.setPixelColor(0, GREEN);
  } else {
    onboard.setPixelColor(0, RED);  
  }
  onboard.show();
  last_reading = reading;
  
  return button_pressed;
}

void enter_adjustment_mode() {
  switch (current_mode) {
    case RANDOM_BLINK:
      rotatePalettes();
      break;
    case HOLIDAY:
      chooseHoliday();
      break;
    case SAFETY:
    case RAINBOW:
      adjustSpeed();
      break;
    case AURA:
      chooseAura();
    default:
      colorSweep();
      break;
  }
}

/*
 * Implement delay() but check button status during delay
 */
boolean delay_and_check(unsigned long ms){
  unsigned long start = millis();

  while((millis() - start) <= ms){
    if(checkButton()) {
      return true;
    }
  }
  return false;
}

void off(){
  clearColor();
  chosen_color = BLUE;  
  leds_on = false;
}


