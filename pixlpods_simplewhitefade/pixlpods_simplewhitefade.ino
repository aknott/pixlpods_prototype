#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 10
#define PXL_PIN 6
#define INPUT_PIN 2

#define DEBOUNCE_INTERVAL 50
#define MODE_LENGTH 1000 //ms
#define OFF_LENGTH 3000 //ms

#define WHITE 0xFFFFFF

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PXL_PIN, NEO_GRB + NEO_KHZ800);

boolean leds_on = false;
boolean adjustment_mode = false;

uint8_t last_reading      = HIGH;
uint8_t last_button_state = HIGH;
uint8_t button_state;

unsigned long off_start = 0;
unsigned long adj_start = 0;
unsigned long debounce_start = 0;

uint8_t current_brightness = 50;

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.setBrightness(current_brightness);
  strip.show();
  pinMode(INPUT_PIN, INPUT_PULLUP);
}

void loop() {
  if (leds_on){
    if (adjustment_mode) {
      enter_adjustment_mode();
    } else {
      setColor(WHITE);  
    }
  } else {
    clearColor();
  }
  checkButton();
}


void enter_adjustment_mode() {
  boolean up = true;
  while (adjustment_mode) {
    current_brightness = up ? current_brightness + 1 : current_brightness - 1;
    strip.setBrightness(current_brightness);
    strip.show();
    if (current_brightness == 100 || current_brightness == 10){
      up = !up;
    }
    delay_and_check(50);
  }
}


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
  
  last_reading = reading;
  
  return button_pressed;
}

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
  leds_on = false;
}


