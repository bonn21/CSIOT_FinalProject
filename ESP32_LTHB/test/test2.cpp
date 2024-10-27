#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
// #include <MFRC522.h>
// #include <SPI.h>
#include <DHT.h>
#include <DHT_U.h>
#include <user_rfid.h>
#include <network.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

#define RL1 14
#define RL2 25
#define RL3 26
#define RL4 27
#define RL5 33
#define RLON LOW
#define RLOFF HIGH
#define PIN_MQ2 32
#define SERVO1 12
#define SERVO1 13

#define DHT11_DATA 15
#define DHTTYPE DHT11 // DHT 22 (AM2302)

DHT_Unified dht(DHT11_DATA, DHTTYPE);
uint8_t _dht_temp = 0;
uint8_t _dht_humi = 0;

Servo myservo;

int MQ2_Read();

void setup()
{
  Serial.begin(9600);
  // myservo.attach(13);
  // myservo.attach(12);
  // WIFI_Connect();
  Wifi_Setup();
  // RFID_Init();
}

void loop()
{
  
}

int MQ2_Read()
{
  int value = 0;
  for (int i = 0; i < 10; i++)
  {
    value += analogRead(PIN_MQ2);
  }
  return map(value / 10, 0, 4095, 0, 100);
}