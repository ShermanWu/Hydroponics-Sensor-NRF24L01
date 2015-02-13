#include <JeeLib.h>  // Include library containing low power functions

ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup for low power waiting

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <TBClient.h>


const String systemID = "8a6974ff";

TBClient client((byte *) "hyd01", 32);

byte water_level_sensor_status = 0;
//Set transmit interval in seconds (max 65 seconds)
unsigned long transmit_interval = 60;

void setup()
{
  transmit_interval = transmit_interval * 1000;
  pinMode(5, INPUT_PULLUP);
}

void loop()
{
  byte water_level_sensor_current = digitalRead(5);
  /*
  if (water_level_sensor_status != water_level_sensor_current) {
    water_level_sensor_status = water_level_sensor_current;
    sendDataWithIDAndStatus(systemID, water_level_sensor_status);
  }*/
  sendDataWithIDAndStatus(systemID, water_level_sensor_current);
  Sleepy::loseSomeTime(transmit_interval);
}

void sendDataWithIDAndStatus(String id, byte status)
{
  byte sensorStatus[12];
  id.getBytes(sensorStatus, 11);
  sensorStatus[11] = status;
  client.sendData((byte *)"hydrohub", (byte *)sensorStatus);
  free(sensorStatus);
}
