#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>

#include "FastLED.h"

//Set number of WS2812B LEDs
#define NUM_LEDS 1

//Set data pin for WS2812B LEDs
#define DATA_PIN 8

//Create LED object
CRGB leds[NUM_LEDS];

//Set initial LED colour
char led_colour = 'B';

unsigned long last_transmit_millis = 0;
int failed_trans = 0;

//Set transmit interval in seconds
unsigned long transmit_interval = 60;

//Failed tranmissions limit (for blue disconnect led)
int failed_trans_limit = 5;


void setup()
{
  Serial.begin(9600);
  // add extra time to the interval to account for differences in low power mode
  transmit_interval = transmit_interval * 1000 * 1.3; 
  
  //Add LED config
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  
  //Set Mirf (NRF24L01+) pins
  Mirf.csnPin = 10;
  Mirf.cePin = 9;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  //Set Mirf receive address
  Mirf.setRADDR((byte *) "hydrohub");
  //Set payload size of NRF24L01+ in bytes
  Mirf.payload = 32;
  //Commit Mirf settings
  Mirf.config();
}

void loop()
{
  //Check if transmit interval has elapsed without any transmissions
  if((millis() - last_transmit_millis) > transmit_interval){
    failed_trans = failed_trans + 1;
    last_transmit_millis = millis();
    if(failed_trans >= failed_trans_limit){
      led_colour = 'B';
    }
    Serial.println(failed_trans);
  }
  //Serial.println(millis() - last_transmit_millis);
  
  //If data is received
  if (Mirf.dataReady()) {
    //Log last tranmission time
    last_transmit_millis = millis();
    //Reset failed transmission counter
    failed_trans = 0;
    
    byte data[32];
    Mirf.getData((byte *) &data);
    String id = String((char *)data);
    
    if(data[11] == 1){
      led_colour = 'R';
    }else{
      led_colour = 'G';
    }
  }
  led_show(led_colour);
}



//Function for changing LED colour
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
