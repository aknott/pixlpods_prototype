#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 10
#define PXL_PIN 6
#define INPUT_PIN 2

#define DEBOUNCE_INTERVAL 50
#define MODE_LENGTH 1000 //ms
#define OFF_LENGTH 3000 //ms

#define WHITE 0xFFFFFF
#define INITIAL_COLOR 0x0000FF

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PXL_PIN, NEO_GRB + NEO_KHZ800);

boolean leds_on = false;
boolean adjustment_mode = false;

uint8_t last_reading      = HIGH;
uint8_t last_button_state = HIGH;
uint8_t button_state;

unsigned long off_start = 0;
unsigned long adj_start = 0;
unsigned long debounce_start = 0;

uint32_t chosen_color = INITIAL_COLOR;

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.setBrightness(50);
  strip.show();
  pinMode(INPUT_PIN, INPUT_PULLUP);
}

void loop() {
  if (leds_on){
    if (adjustment_mode) {
      colorSweep();
    } else {
      setColor(chosen_color);  
    }
  } else {
    clearColor();
  }
  checkButton();
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
    delay_and_check(50);
    checkButton();
  }
  chosen_color = color;
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


