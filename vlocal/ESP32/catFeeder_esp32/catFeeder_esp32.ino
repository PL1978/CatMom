#include <ESP32Servo.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "time.h"
#include <setjmp.h>
#include <stdio.h>
#include <string>

jmp_buf buf;

Servo foodValve;

const uint8_t SERVO_PIN = 16;
const uint8_t ALIM_SERVO_PIN = 23;
const uint8_t ERROR_LED_PIN = 25;

const uint8_t OPEN_ANGLE = 16;
const uint8_t CLOSE_ANGLE = 35;

const char* ESTERN_TZ = "EST5EDT,M3.2.0,M11.1.0";
const char* NTP_SERVER = "pool.ntp.org";
const char* SSID = "FIZZ84084";
const char* PSWD = "Colocation21*";

IPAddress local_IP(192, 168, 0, 184);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(1, 1, 1, 1);

AsyncWebServer server(80);

typedef struct scheduledMeal {
  uint8_t mealHour;
  ushort openTime;
} scheduledMeal_t;

void signalError() {
  digitalWrite(ERROR_LED_PIN, HIGH);
}

void clearError() {
  digitalWrite(ERROR_LED_PIN, LOW);
}

void feed(ushort openAngle, ushort closeAngle, uint openTime) {
  digitalWrite(ALIM_SERVO_PIN, HIGH);
  foodValve.write(openAngle);
  delay(openTime);
  foodValve.write(closeAngle);
  delay(200);

  digitalWrite(ALIM_SERVO_PIN, LOW);
}

std::string failedBodyField = "";

template <size_t capacity>
JsonVariant getJsonValue(const char* key, StaticJsonDocument<capacity>& document) {
  if (document.containsKey(key)) {
    return document[key];
  } else {
    failedBodyField = key;
    Serial.print("unexpected body missing key ");
    Serial.println(key);
    signalError();
    longjmp(buf, 1);
  }
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(SSID, PSWD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(" ");
  }
  
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  }
  

  pinMode(ALIM_SERVO_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  foodValve.attach(SERVO_PIN);

  configTime(0, 0, NTP_SERVER);
  setenv("TZ", ESTERN_TZ, 1); // Change this to your timezone string
  tzset();
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  } else {
    Serial.print("Year: ");
    Serial.println(timeinfo.tm_year + 1900);
    Serial.print("Month: ");
    Serial.println(timeinfo.tm_mon + 1);
    Serial.print("Day: ");
    Serial.println(timeinfo.tm_mday);
    Serial.print("Hour: ");
    Serial.println(timeinfo.tm_hour);
    Serial.print("Minute: ");
    Serial.println(timeinfo.tm_min);
    Serial.print("Second: ");
    Serial.println(timeinfo.tm_sec);
  }
  server.on("/api/meal", HTTP_POST, [] (AsyncWebServerRequest * request) {}, NULL, [] (AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    StaticJsonDocument<64> mealBody;
    std::string body(reinterpret_cast<char*>(data), len);
    scheduledMeal_t postedMeal;

    if (setjmp(buf)) {
      request->send(400, "text/plain", (std::string("Missing field ") + failedBodyField).c_str());
    } else {
      DeserializationError error = deserializeJson(mealBody, body);
      if (error) {
        request->send(500, "text/plain", (std::string("Error while parsing json body: ") + error.c_str()).c_str());
      } else {
        postedMeal.mealHour = getJsonValue("mealHour", mealBody).as<uint8_t>();
        postedMeal.openTime = getJsonValue("openTime", mealBody).as<ushort>();
        Serial.println(postedMeal.mealHour);
        Serial.println(postedMeal.openTime);
        request->send(200);
      }
    }
    mealBody.clear();
    
  });

  server.begin();
}

void loop() {
}
