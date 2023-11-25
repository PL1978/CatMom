#include <ESP32Servo.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <setjmp.h>
#include <stdio.h>
#include <string>
#include <Preferences.h>
#include <ctime>

/**
- extraire la logique des endpoints action et log
- get time retries
- écrire dans la flash
*/

typedef struct scheduledMeal scheduledMeal_t;
typedef struct dailyMeal dailyMeal_t;
typedef struct log log_t;

Preferences flash;
jmp_buf buf;

Servo foodValve;

const uint8_t SERVO_PIN = 16;
const uint8_t ALIM_SERVO_PIN = 23;
const uint8_t ERROR_LED_PIN = 25;

const uint8_t OPEN_ANGLE = 16;
const uint8_t CLOSE_ANGLE = 35;

const char* SSID = "FIZZ84084";
const char* PSWD = "Colocation21*";
const size_t MEAL_BODY_SIZE = 64;

IPAddress local_IP(192, 168, 0, 184);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(1, 1, 1, 1);

AsyncWebServer server(80);

const char* FS_GROUP = "CATMOM";
const char* FS_MEAL_NB = "mealCount";
const char* FS_MEALS = "meals";

void feed(ushort openTime) {
  digitalWrite(ALIM_SERVO_PIN, HIGH);
  foodValve.write(OPEN_ANGLE);
  delay(openTime);
  foodValve.write(CLOSE_ANGLE);
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
    signalError("EKEY");
    longjmp(buf, 1);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ALIM_SERVO_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  foodValve.attach(SERVO_PIN);

  WiFi.begin(SSID, PSWD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(" ");
  }
  Serial.print("\n");
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  } else {
    Serial.println(WiFi.localIP());
  }
  struct tm currentTime;
  if(adjustRTC(currentTime, false)) {
    loadMealFromFlash(currentTime);
  }
  
  server.on("/api/meal", HTTP_POST, [] (AsyncWebServerRequest * request) {}, NULL, handlePostMeal);
  server.on("/api/meal", HTTP_DELETE, handleDeleteMeal);
  server.on("/api/meal", HTTP_GET, handleGetMeal);

  server.on("/api/log/error", HTTP_GET, handleGetErrorLog);
  server.on("/api/log/action", HTTP_GET, handleGetActionLog);

  server.begin();
  Serial.println("ready");
}

void loop() {
  delay(5000);
  struct tm currentTime;
  if(setCurrentTime(currentTime)) { 
    adjustRTCIfNeeded(currentTime);
    feedIfMealDue(currentTime);
  }
}
