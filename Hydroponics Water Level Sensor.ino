#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>

#include <TBClient.h>
#include <TBWrapper.h>

const String systemID = "8a6974ff";

TBClient client((byte *) "hyd01", 32);

byte water_level_sensor_status = 0;


void clientSetup()
{
  pinMode(5, INPUT_PULLUP);
  digitalWrite(2, HIGH);
}

void clientLoop()
{
  byte water_level_sensor_current = digitalRead(5);

  if (water_level_sensor_status != water_level_sensor_current) {
    water_level_sensor_status = water_level_sensor_current;
    sendDataWithIDAndStatus(systemID, water_level_sensor_status);
  }

}

void sendDataWithIDAndStatus(String id, byte status)
{
  byte sensorStatus[12];
  id.getBytes(sensorStatus, 11);
  doorStatus[11] = status;
  client.sendData((byte *)"tbhub", (byte *)doorStatus);
  free(doorStatus);
}