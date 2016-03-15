#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 10
#define PXL_PIN 6
#define INPUT_PIN 2

#define DEBOUNCE_INTERVAL 50

#define WHITE 0xFFFFFF

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PXL_PIN, NEO_GRB + NEO_KHZ800);

boolean leds_on = false;

uint8_t last_reading      = HIGH;
uint8_t last_button_state = HIGH;
uint8_t button_state      = HIGH;

unsigned long debounce_start = 0;

void setup() {
  strip.begin();
  strip.setBrightness(50);
  strip.show();
  pinMode(INPUT_PIN, INPUT_PULLUP);
}

void loop() {
  if (leds_on){
    setColor(WHITE);
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

void checkButton(){
  uint8_t reading = digitalRead(INPUT_PIN);
  
  if (reading != last_reading) {
    debounce_start = millis(); 
  }

  if ((millis() - debounce_start) > DEBOUNCE_INTERVAL) {
    if (reading != button_state) {
      button_state = reading;
      if (button_state == HIGH && last_button_state == LOW) {
        leds_on = !leds_on;
      }
      last_button_state = button_state;  
    }
  }
  last_reading = reading;
}

