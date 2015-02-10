#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>

#include "FastLED.h"

#define NUM_LEDS 1
#define DATA_PIN 8
CRGB leds[NUM_LEDS];
char led_colour = 'B';

void setup()
{
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  Mirf.csnPin = 10;
  Mirf.cePin = 9;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  Mirf.setRADDR((byte *) "hydrohub");
  Mirf.payload = 32;

  Mirf.config();

 
}

void loop()
{
  if (Mirf.dataReady()) {
    byte data[32];
    Mirf.getData((byte *) &data);
    String id = String((char *)data);
    Serial.println();
    if(data[11] == 1){
      led_colour = 'R';
    }else{
      led_colour = 'G';
    }
  }
  led_show(led_colour);
}

void led_show(char colour){
  switch(colour){
  case 'R':
  leds[0] = CRGB::Red;
  break;
  case 'G':
  leds[0] = CRGB::Green;
  break; 
  case 'B':
  leds[0] = CRGB::Blue;
  break; 
  }
  
  FastLED.show();
}
