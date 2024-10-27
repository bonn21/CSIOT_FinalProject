// #include <Arduino.h>
// #include <Wire.h>
// #include <ArduinoJson.h>
// #include <ESP32Servo.h>
// #include <MFRC522.h>
// #include <SPI.h>
// #include <DHT.h>
// #include <DHT_U.h>

// #define RL1   14
// #define RL2   25
// #define RL3   26
// #define RL4   27
// #define RL5   33
// #define RLON  LOW
// #define RLOFF HIGH
 

// #define DHT11_DATA 15
// #define DHTTYPE DHT11 // DHT 22 (AM2302)
// DHT_Unified dht(DHT11_DATA, DHTTYPE);
// uint8_t _dht_temp = 0;
// uint8_t _dht_humi = 0;

// #define RST_PIN         9          // Configurable, see typical pin layout above
// #define SS_1_PIN        10         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
// #define SS_2_PIN        8          // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1


// void Dht11_Init();
// void Dht11_Read();

// void setup()
// {
//   Serial.begin(115200);
//   pinMode(RL1, OUTPUT);
//   pinMode(RL2, OUTPUT);
//   pinMode(RL3, OUTPUT);
//   pinMode(RL4, OUTPUT);
//   pinMode(RL5, OUTPUT);
//   Dht11_Init();
  
// }

// void loop()
// {
//   Dht11_Read();
//   delay(2000);
// }

// void Relay_Init(){
//   pinMode(RL1, OUTPUT);
//   pinMode(RL2, OUTPUT);
//   pinMode(RL3, OUTPUT);
//   pinMode(RL4, OUTPUT);
//   pinMode(RL5, OUTPUT);
//   digitalWrite(RL1, RLOFF);
//   digitalWrite(RL2, RLOFF);
//   digitalWrite(RL3, RLOFF);
//   digitalWrite(RL4, RLOFF);
//   digitalWrite(RL5, RLOFF);
// }

// void Dht11_Init()
// {
//   dht.begin();

//   sensor_t sensor;
//   dht.temperature().getSensor(&sensor);

//   dht.humidity().getSensor(&sensor);
// }

// void Dht11_Read()
// {
//   sensors_event_t event;
//   dht.temperature().getEvent(&event);

//   Serial.print(F("Temperature: "));
//   _dht_temp = event.temperature;
//   Serial.print(_dht_temp);
//   Serial.println(F("*C"));

//   dht.humidity().getEvent(&event);
//   Serial.print(F("Humidity: "));
//   _dht_humi = event.relative_humidity;
//   Serial.print(_dht_humi);
//   Serial.println(F("%"));
// }
