// #include <Arduino.h>
// #include <Wire.h>
// #include <ArduinoJson.h>
// #include <ESP32Servo.h>
// // #include <MFRC522.h>
// // #include <SPI.h>
// #include <DHT.h>
// #include <DHT_U.h>
// #include <user_rfid.h>
// #include <network.h>
// #include "ESPAsyncWebServer.h"
// #include "SPIFFS.h"
// #include <index_html.h>

// #define RL1 14
// #define RL2 25
// #define RL3 26
// #define RL4 27
// #define RL5 33
// #define RLON LOW
// #define RLOFF HIGH
// #define PIN_MQ2 32
// #define SERVO1 12
// #define SERVO2 13
// #define PIN_RAIN 34
// #define PIN_SOILD 35

// #define DHT11_DATA 15
// #define DHTTYPE DHT11 // DHT 22 (AM2302)
// sensor_t sensor;
// sensors_event_t event;

// AsyncWebServer server(80); // port 80

// DHT_Unified dht(DHT11_DATA, DHTTYPE);
// uint8_t _dht_temp = 0;
// uint8_t _dht_humi = 0;
// uint8_t _soil = 0;
// uint8_t mode_auto = 1;


// Servo myservo1, myservo2;

// typedef struct
// {
//   uint8_t balcony_led = 0;
//   uint8_t balcony_door = 0;
//   uint8_t living_led = 0;
//   uint8_t living_fan = 0;
//   uint8_t kitchen_fan = 0;
//   uint8_t bedroom_led = 0;
//   uint8_t garden_pump = 0;
// } Dev;

// int value_mq2 = 0;

// String MQ2_Read();
// String Rain_Read();
// String readDHTTemperature();
// String Soild_Read();
// String readDHTHumidity();
// int Soil_Read();

// Dev deviec;
// void Dht11_Init();

// String processor(const String &var)
// {
//   if (var == "TEMPERATURE")
//   {
//     return readDHTTemperature();
//   }
//   else if (var == "HUMIDITY")
//   {
//     return readDHTHumidity();
//   }
//   else if (var == "GAS"){
//     return MQ2_Read();
//   }
//   return String();
// }

// void setup()
// {
//   Serial.begin(115200);
//   Wifi_Setup();
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
//   Dht11_Init();
//   myservo1.attach(SERVO1);
//   myservo2.attach(SERVO2);
//   // WIFI_Connect();
//   // Wifi_Setup();
//   RFID_Init();
//   if (!SPIFFS.begin(true)) // kiểm tra trang thái hoạt động của SPIFFS
//   {
//     Serial.println("An Error has occurred while mounting SPIFFS");
//     return;
//   }
//   // Wifi_Setup();
//   //   // đường dẫn khi mở web, ấn f12 để hiểu hơn về hàm
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     // flag_manual = 0; // mặc định quạt chạy ở chế độ auto
//     request->send_P(200, "text/html", index_html, processor); });

//   server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
//             { request->send_P(200, "text/plain", readDHTTemperature().c_str()); });

//   // lấy thông tin độ ẩm
//   server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request)
//             { request->send_P(200, "text/plain", readDHTHumidity().c_str()); });

//   // Mode auto
//    server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     mode_auto = 1;
//     request->send_P(200, "text/html", index_html, processor); });

//   // khu vực hành lang, ban công
//   server.on("/manual/balcony/ledon", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.balcony_led = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/balcony/ledoff", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.balcony_led = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/balcony/open_door", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.balcony_door = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/balcony/cloes_door", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.balcony_door = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });

//   // Khu vực phòng khách - bếp
//   server.on("/manual/livingroom/ledon", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.living_led = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/livingroom/ledoff", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.living_led = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   //------------------------------
//   server.on("/manual/livingroom/fanon", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.living_fan = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/livingroom/fanoff", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.living_fan = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   //------------------------------
//   server.on("/manual/kitchen/fanon", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.kitchen_fan = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/kitchen/fanoff", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.kitchen_fan = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });

//   // khu vực phòng ngủ
//   server.on("/manual/bedroom/ledon", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.bedroom_led = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/bedroom/ledoff", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.bedroom_led = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });

//   // khu vực vườn
//   server.on("/manual/garden/pumpon", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.garden_pump = 1;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.on("/manual/garden/pumpoff", HTTP_GET, [](AsyncWebServerRequest *request)
//             {
//     deviec.garden_pump = 0;
//     mode_auto = 0;
//     request->send_P(200, "text/html", index_html, processor); });
//   server.begin();
// }

// void loop()
// {
//   Serial.print(F("Temperature: "));
//   Serial.print(_dht_temp);
//   Serial.println(F("*C"));
//   Serial.print(F("Humidity: "));
//   Serial.print(_dht_humi);
//   Serial.println(F("%"));

//   if(mode_auto){
//     Serial.println("Enter Mode Auto");
//     if(value_mq2 >= 20){ // cảnh bảo khí gas trong phòng bếp
//       // digitalWrite(RL1, RLON);
//       Serial.println("canh bao Mq2 + chuong");
//     }
//     else{
//       // digitalWrite(RL1, RLOFF);
//     }
//     if(_dht_temp >= 29){
//       Serial.println("bat quat phong khach");
//     }
//     else{
//       Serial.println("tat quat phong khach");
//     }
//     if(_soil < 60){
//       Serial.println("bat may bom");
//     }
//     else{
//       Serial.println("tat may bom");
//     }
//   }
//   else{
//     Serial.println("Exit Mode Auto");
//   }
//   // khu vực hành lang
//   if (deviec.balcony_led)
//   {
//     digitalWrite(RL1, RLON);
//     // Serial.println("ON LED Balcony.");
//   }
//   else
//   {
//     digitalWrite(RL1, RLOFF);
//     // Serial.println("OFF LED Balcony.");
//   }
//   if (deviec.balcony_door || flag_open_door == 1)
//   {
//     // digitalWrite(RL1, RLON);
//     myservo1.write(100);
//     Serial.println("open the door Balcony.");
//   }
//   else
//   {
//     myservo1.write(0);
//     Serial.println("close the door Balcony.");
//   }

//   // khu vực phòng khách
//   if (deviec.living_led)
//   {
//     digitalWrite(RL2, RLON);
//     // Serial.println("ON LED Livingroom.");
//   }
//   else
//   {
//     digitalWrite(RL2, RLOFF);
//     // Serial.println("OFF LED Livingroom.");
//   }
//   if (deviec.living_fan)
//   {
//     digitalWrite(RL3, RLON);
//     // Serial.println("ON FAN Livingroom.");
//   }
//   else
//   {
//     digitalWrite(RL3, RLOFF);
//     // Serial.println("OFF FAN Livingroom.");
//   }
//   if (deviec.kitchen_fan)
//   {
//     digitalWrite(RL4, RLON);
//     // Serial.println("ON FAN kitchen.");
//   }
//   else
//   {
//     digitalWrite(RL4, RLOFF);
//     // Serial.println("OFF FAN kitchen.");
//   }

//   // Khu vực phòng ngủ
//   if (deviec.bedroom_led)
//   {
//     digitalWrite(RL5, RLON);
//     // Serial.println("ON LED Bedroom.");
//   }
//   else
//   {
//     digitalWrite(RL5, RLOFF);
//     // Serial.println("OFF LED Bedroom.");
//   }

//   // khu vực vườn
//   if (deviec.garden_pump)
//   {
//     // digitalWrite(RL5, RLON);
//     Serial.println("ON PUMP Garden.");
//   }
//   else
//   {
//     // digitalWrite(RL5, RLOFF);
//     Serial.println("OFF PUMP Garden.");
//   }

//   delay(200);
// }

// void Dht11_Init()
// {
//   dht.begin();
//   dht.temperature().getSensor(&sensor);
//   dht.humidity().getSensor(&sensor);
// }

// String MQ2_Read()
// {
//   int value = 0;
//   for (int i = 0; i < 10; i++)
//   {
//     value += analogRead(PIN_MQ2);
//   }
//   value_mq2 = value;
//   return String(map(value / 10, 0, 4095, 0, 100));
// }

// String Rain_Read()
// {
//   return String(digitalRead(PIN_RAIN));
// }

// String Soild_Read()
// {
//   return String(digitalRead(PIN_SOILD));
// }

// String readDHTTemperature()
// {
//   dht.temperature().getEvent(&event);
//   _dht_temp = event.temperature;
//   return String(_dht_temp);
// }

// String readDHTHumidity() // đọc độ ẩm và trả lại dưới dạng chuỗi
// {
//   dht.humidity().getEvent(&event);
//   _dht_humi = event.relative_humidity;
//   return String(_dht_humi);
// }

// int Soil_Read(){
//   int value = 0;
//   for(int i = 0; i < 10; i++){
//     value += analogRead(PIN_SOILD);
//   }
//   _soil = value/10;
//   return map(value/10, 0, 4095, 0, 100);
// }