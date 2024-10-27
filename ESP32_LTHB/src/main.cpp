// Thư viện
#include <Arduino.h> 
#include <Wire.h> 
#include <ArduinoJson.h> 
#include <ESP32Servo.h> 
#include <DHT.h> 
#include <DHT_U.h>
#include <user_rfid.h> 
#include <network.h>
#include "ESPAsyncWebServer.h" 
#include "SPIFFS.h"
#include <index_html.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <time.h>

#define API_KEY "AIzaSyBeuI7ocCgI5g1d69oxiEi6rtAzOlmPBSE"
#define DATABASE_URL "https://smarthome-36cb0-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Setup chân
#define RL2         16 //Led khách - IR2 - 25
#define RL3         26 // quạt khách - DHT11 - 26 - TRUE
#define RL4         14 // quạt bếp - MQ2 - 27
#define RL5         33 // led nguồn - IR2 - 33 - TRUE
#define RL6         25 // còi
#define RLON        LOW
#define RLOFF       HIGH
#define PIN_MQ2     32
#define SERVO1      12
#define PIN_SOILD   35
#define POS_OPEN    0
#define POS_CLOSE   45
#define DHT11_DATA  15
#define DHTTYPE     DHT11 // DHT 22 (AM2302)
sensor_t sensor;
sensors_event_t event;

AsyncWebServer server(80); // port 80

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int dataID = 1; // Initialize data ID
float tempValue;
float humidityValue;

// Tạo biến dữ liệu cho các cảm biến
DHT_Unified dht(DHT11_DATA, DHTTYPE);
int _dht_temp = 0; // nhiệt độ
int _dht_humi = 0; // độ ẩm
uint8_t _soil = 0; // độ ẩm đất
uint8_t mode_auto = 1; // chế độ tự động

Servo myservo1;

// Setup chạng thái của các thiết bị
typedef struct
{
  // 0: tắt, 1: bật
  uint8_t balcony_door = 0; 
  uint8_t living_led = 0;
  uint8_t living_fan = 0;
  uint8_t kitchen_fan = 0;
  uint8_t bedroom_led = 0;
  uint8_t garden_buzzer = 0;
} Dev;

int value_mq2 = 0;
// Khai báo timer0
hw_timer_t *Timer0_Cfg = NULL; 
uint32_t Freq = 0; //
int cnt = 0; //

uint8_t flag_stt = 0; 
// Hàm ngắt
void IRAM_ATTR Blink_Led() 
{
  flag_stt = 1; 
}
// khai báo các hàm
void Interrupt_Init(); 
void Web_Control(); 
void Operate_System();
String MQ2_Read();
String readDHTTemperature();
String Soild_Read();
String readDHTHumidity();
int Soil_Read(); 

Dev deviec; 
void Dht11_Init();
void Gpio_Init();

// các biến cho các thiết bị
uint8_t flag_check_rf = 0; 
uint8_t mode_select_door = 0;
uint8_t mode_control_device = 0;
// kiểm tra các biến được chuyền vào
String processor(const String &var) 
{
  if (var == "TEMPERATURE") 
  {
    return readDHTTemperature();
  }
  else if (var == "HUMIDITY") 
  {
    return readDHTHumidity();
  }
  else if (var == "GAS")
  {
    return MQ2_Read();
  }
  return String();
}

// hàm chính
void setup()
{
  Serial.begin(115200); 
  Interrupt_Init(); 
  Gpio_Init(); 
  Dht11_Init(); 
  myservo1.attach(SERVO1);
  RFID_Init();
  if (!SPIFFS.begin(true)) // kiểm tra trang thái hoạt động của SPIFFS
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Wifi_Setup();
  Web_Control();

  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Waiting for NTP time sync: ");
    while (!time(nullptr)) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println();
    Serial.println("Time synchronized");

  // Firebase configuration
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    // Sign up for Firebase authentication
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Firebase signup successful");
        signupOK = true;
    } else {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Get the current highest ID from Firebase
    if (Firebase.RTDB.getInt(&fbdo, "/DHTData/lastID")) {
        dataID = fbdo.intData() + 1;
    } else {
        dataID = 1;
    }
    server.begin();

}

// vòng lặp
void loop()
{
  if (flag_stt) 
  {
    flag_check_rf = 1; 
    cnt++;
    flag_stt = 0;
  }

  if (flag_check_rf)
  {
    RFID_Check();
    flag_check_rf = 0;
  }
  if (flag_open_door == 1)
  {
     mode_select_door = 1;
    deviec.balcony_door = 1;
    Firebase.RTDB.setFloat(&fbdo, "/command/balcony_door", deviec.balcony_door);
    myservo1.write(POS_OPEN);
    Serial.println("OPEN");

    delay(5000); // Wait for 5 seconds

    deviec.balcony_door = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/balcony_door", deviec.balcony_door);
    myservo1.write(POS_CLOSE);
    Serial.println("CLOSE");

    flag_open_door = 0;
    mode_select_door = 0;
  }

  if (mode_auto)
  {
    if (_dht_temp >= 27 && _dht_humi <= 100)
    {
      deviec.living_fan = 1;
      Firebase.RTDB.setFloat(&fbdo, "/command/living_fan", deviec.living_fan);
      digitalWrite(RL3, RLON);
    }
    else
    {
      deviec.living_fan = 0;
      digitalWrite(RL3, RLOFF);
      Firebase.RTDB.setFloat(&fbdo, "/command/living_fan", deviec.living_fan);
    }

    if (value_mq2 > 3)
    {
      deviec.kitchen_fan = 1;
      digitalWrite(RL4, RLON);
      Firebase.RTDB.setFloat(&fbdo, "/command/kitchen_fan", deviec.kitchen_fan);
      deviec.garden_buzzer = 1;
      Firebase.RTDB.setFloat(&fbdo, "/command/garden_buzzer", deviec.garden_buzzer);
      digitalWrite(RL6, RLON);
    }
    else
    {
      deviec.kitchen_fan = 0;
      Firebase.RTDB.setFloat(&fbdo, "/command/kitchen_fan", deviec.kitchen_fan);
      digitalWrite(RL4, RLOFF);
      deviec.garden_buzzer = 0;
      Firebase.RTDB.setFloat(&fbdo, "/command/garden_buzzer", deviec.garden_buzzer);
      digitalWrite(RL6, RLOFF);
    }
  }

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getFloat(&fbdo, "/command/bedroom_led")) {
        deviec.bedroom_led = fbdo.floatData();
        Serial.print("bedroom_led: ");
        Serial.println(deviec.bedroom_led);
    } else {
        Serial.print("Failed to get bedroom_led: ");
        Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getFloat(&fbdo, "/command/kitchen_fan")) {
        deviec.kitchen_fan = fbdo.floatData();
        Serial.print("kitchen_fan: ");
        Serial.println(deviec.kitchen_fan);
    } else {
        Serial.print("Failed to get kitchen_fan: ");
        Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getFloat(&fbdo, "/command/living_fan")) {
        deviec.living_fan = fbdo.floatData();
        Serial.print("living_fan: ");
        Serial.println(deviec.living_fan);
    } else {
        Serial.print("Failed to get temperature: ");
        Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getFloat(&fbdo, "/command/living_led")) {
        deviec.living_led = fbdo.floatData();
        Serial.print("living_led: ");
        Serial.println(deviec.living_led);
    } else {
        Serial.print("Failed to get living_led: ");
        Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getFloat(&fbdo, "/command/garden_buzzer")) {
        deviec.garden_buzzer = fbdo.floatData();
        Serial.print("garden_buzzer: ");
        Serial.println(deviec.garden_buzzer);
    } else {
        Serial.print("Failed to get garden_buzzer: ");
        Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getFloat(&fbdo, "/command/balcony_door")) {
        deviec.balcony_door = fbdo.floatData();
        if(deviec.balcony_door == 1){
          myservo1.write(POS_OPEN);
        } else {
          myservo1.write(POS_CLOSE);
        }
        Serial.print("balcony_door: ");
        Serial.println(deviec.balcony_door);
    } else {
        Serial.print("Failed to get balcony_door: ");
        Serial.println(fbdo.errorReason());
    }
        // Print values to Serial Monitor
        // Serial.print("Temperature: ");
        // Serial.print(readDHTTemperature());
        // Serial.println(" °C");

        // Serial.print("Humidity: ");
        // Serial.print(readDHTHumidity());
        // Serial.println(" %");

        // Serial.print("Soil Moisture: ");
        // Serial.print(Soil_Read());
        // Serial.println(" %");

        // Serial.print("Gas: ");
        // Serial.print(MQ2_Read());
        // Serial.println(" %");

        // Get current timestamp
      time_t now = time(nullptr);
      struct tm* timeinfo = localtime(&now);
      char timestamp[20];
      strftime(timestamp, sizeof(timestamp), "%H:%M:%S %d/%m/%Y", timeinfo);
      
      // Create JSON object to send
      FirebaseJson datajson;
      datajson.set("temperature", readDHTTemperature());
      datajson.set("humidity", readDHTHumidity());
      // datajson.set("soil", Soil_Read());
      datajson.set("gas", MQ2_Read());
      datajson.set("timestamp", timestamp);

      String datapath = "/DHTData/";
      datapath.concat(String(dataID));
      if (Firebase.RTDB.setJSON(&fbdo, datapath.c_str(), &datajson)) {
        Serial.println("Data sent to Firebase");
        Firebase.RTDB.setInt(&fbdo, "/DHTData/lastID", dataID); // Save the last ID
        dataID++; // Increment data ID
      } else {
        Serial.println(fbdo.errorReason());
      }
    }

  Operate_System();
  //delay(2000); // Delay to prevent flooding the serial monitor
}

void Interrupt_Init()
{
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &Blink_Led, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
}

void Gpio_Init()
{
  pinMode(RL2, OUTPUT);
  pinMode(RL3, OUTPUT);
  pinMode(RL4, OUTPUT);
  pinMode(RL5, OUTPUT);
  pinMode(RL6, OUTPUT);
  digitalWrite(RL2, RLOFF);
  digitalWrite(RL3, RLOFF);
  digitalWrite(RL4, RLOFF);
  digitalWrite(RL5, RLOFF);
  digitalWrite(RL6, RLOFF);
}

void Dht11_Init()
{
  dht.begin();
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
}

String MQ2_Read()
{
  int value = 0;
  for (int i = 0; i < 10; i++)
  {
    value += analogRead(PIN_MQ2);
  }
  value_mq2 = value;
  return String(map(value / 10, 0, 4095, 0, 100));
}

float lastValidTemp = 0;
float lastValidHumi = 0;

String readDHTTemperature() {
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature) && event.temperature < 100) { // Check for valid reading
    lastValidTemp = event.temperature;
  }
  _dht_temp = lastValidTemp;
  return String(_dht_temp);
}

String readDHTHumidity() {
  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity) && event.relative_humidity < 100) { // Check for valid reading
    lastValidHumi = event.relative_humidity;
  }
  _dht_humi = lastValidHumi;
  return String(_dht_humi);
}

/***********************************************************************/

void Web_Control()
{
  // đường dẫn khi mở web, ấn f12 để hiểu hơn về hàm
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send_P(200, "text/plain", readDHTTemperature().c_str()); 
  });

  // lấy thông tin độ ẩm
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send_P(200, "text/plain", readDHTHumidity().c_str()); 
  });

  server.on("/gas", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send_P(200, "text/plain", MQ2_Read().c_str()); 
  });

  // Mode auto
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    mode_auto = 1;
    request->send_P(200, "text/html", index_html, processor); 
  });

  // khu vực hành lang, ban công
  server.on("/manual/balcony/open_door", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.balcony_door = 1;
    mode_select_door = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/balcony_door", deviec.balcony_door);
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/manual/balcony/cloes_door", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.balcony_door = 0;
    mode_select_door = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/balcony_door", deviec.balcony_door);
    request->send_P(200, "text/html", index_html, processor); 
  });

  // Khu vực phòng khách - bếp
  server.on("/manual/livingroom/ledon", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.living_led = 1;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/living_led", deviec.living_led);
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/manual/livingroom/ledoff", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.living_led = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/living_led", deviec.living_led);
    request->send_P(200, "text/html", index_html, processor); 
  });

  //------------------------------
  server.on("/manual/livingroom/fanon", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.living_fan = 1;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/living_fan", deviec.living_fan);
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/manual/livingroom/fanoff", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.living_fan = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/living_fan", deviec.living_fan);
    request->send_P(200, "text/html", index_html, processor); 
  });

  //------------------------------
  server.on("/manual/kitchen/fanon", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.kitchen_fan = 1;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/kitchen_fan", deviec.kitchen_fan);
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/manual/kitchen/fanoff", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.kitchen_fan = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/kitchen_fan", deviec.kitchen_fan);
    request->send_P(200, "text/html", index_html, processor); 
  });

  // khu vực phòng ngủ
  server.on("/manual/bedroom/ledon", HTTP_GET, [](AsyncWebServerRequest *request) {
    deviec.bedroom_led = 1;
    mode_control_device = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/bedroom_led", deviec.bedroom_led);
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/manual/bedroom/ledoff", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.bedroom_led = 0;
    mode_control_device = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/bedroom_led", deviec.bedroom_led);
    request->send_P(200, "text/html", index_html, processor); 
  });

  //khu vực vườn
  server.on("/manual/garden/pumpon", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.garden_buzzer = 1; 
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/garden_buzzer", deviec.garden_buzzer);
    request->send_P(200, "text/html", index_html, processor); 
  });

  server.on("/manual/garden/pumpoff", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    deviec.garden_buzzer = 0;
    mode_auto = 0;
    Firebase.RTDB.setFloat(&fbdo, "/command/garden_buzzer", deviec.garden_buzzer);
    request->send_P(200, "text/html", index_html, processor); 
  });
  
  server.begin();
}

void Operate_System()
{
  // khu vực hành lang
  if (deviec.balcony_door == 1 && mode_select_door == 0)
  {
    myservo1.write(POS_OPEN);
  }
  else if (deviec.balcony_door == 0 && mode_select_door == 0)
  {
    mode_select_door = 1;
    myservo1.write(POS_CLOSE);
  }

  // khu vực phòng khách
  if (deviec.living_led == 1 && mode_auto == 0)
  {
    digitalWrite(RL2, RLON);
  }
  else if (deviec.living_led == 0 && mode_auto == 0)
  {
    digitalWrite(RL2, RLOFF);
  }
  if (deviec.living_fan == 1 && mode_auto == 0)
  {
    digitalWrite(RL3, RLON);
  }
  else if (deviec.living_fan == 0 && mode_auto == 0)
  {
    digitalWrite(RL3, RLOFF);
  }

  // khu vực phòng bếp
  if (deviec.kitchen_fan == 1 && mode_auto == 0)
  {
    digitalWrite(RL4, RLON);
  }
  else if (deviec.kitchen_fan == 0 && mode_auto == 0)
  {
    digitalWrite(RL4, RLOFF);
  }

  // Khu vực phòng ngủ
  if (deviec.bedroom_led == 1 && mode_auto == 0)
  {
    digitalWrite(RL5, RLON);
  }
  else if (deviec.bedroom_led == 0 && mode_auto == 0)
  {
    digitalWrite(RL5, RLOFF);
  }

  // khu vực vườn
  if (deviec.garden_buzzer == 1 && mode_auto == 0)
  {
    digitalWrite(RL6, RLON);
  }
  else if (deviec.garden_buzzer == 0 && mode_auto == 0)
  {
    digitalWrite(RL6, RLOFF);
  }
}
