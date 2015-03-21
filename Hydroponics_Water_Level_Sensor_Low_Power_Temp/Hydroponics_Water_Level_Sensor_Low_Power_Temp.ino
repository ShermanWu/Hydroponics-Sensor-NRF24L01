/*-----( Import needed libraries )-----*/
#include <JeeLib.h>  // Include library containing low power functions

#include <OneWire.h>
#include <DallasTemperature.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <TBClient.h>


/*-----( Declare Constants )-----*/
#define ONE_WIRE_BUS 4 //Set pin 4 as one wire bus

/*-----( Declare objects )-----*/
/* Set up a oneWire instance to communicate with any OneWire device*/
OneWire ourWire(ONE_WIRE_BUS);

/* Tell Dallas Temperature Library to use oneWire Library */
DallasTemperature sensors(&ourWire);

ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup for low power waiting

//Sensor pin in pullup mode
int sensor_pin = 6;

//CRC32 hash of system ID e.g hash of "Hydroponics Station LPT-1" is "32a4b6f0" 
const String systemID = "32a4b6f0";

TBClient client((byte *) "hyd01", 32);

//Union type
typedef union {
 float floatingPoint;
 byte binary[4];
} binaryFloat;

binaryFloat temp;

byte water_level_sensor_status = 0;
//Set transmit interval in seconds (max 65 seconds)
unsigned long transmit_interval = 60;

void setup()
{
  transmit_interval = transmit_interval * 1000;
  pinMode(sensor_pin, INPUT_PULLUP);
  
  sensors.begin(); //Start up the DallasTemperature library
}

void loop()
{
  byte water_level_sensor_current = digitalRead(sensor_pin);
  //Get temperature from DS18B20
  sensors.requestTemperatures();
  //Convert sensor value to Celcius
  temp.floatingPoint = sensors.getTempCByIndex(0);
  sendDataWithIDAndStatus(systemID, water_level_sensor_current);
  Sleepy::loseSomeTime(transmit_interval);
}

void sendDataWithIDAndStatus(String id, byte status)
{
  byte sensorStatus[16];
  id.getBytes(sensorStatus, 11);
  sensorStatus[11] = status;
  for(int i = 12; i < 16;i++){
  sensorStatus[i] = temp.binary[i-12];
  }
  
  client.sendData((byte *)"hydrohub", (byte *)sensorStatus);
  free(sensorStatus);
}
