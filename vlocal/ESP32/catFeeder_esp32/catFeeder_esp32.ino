#include <ESP32Servo.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <setjmp.h>
#include <stdio.h>
#include <string>
#include <ctime>

typedef struct scheduledMeal scheduledMeal_t;
typedef struct dailyMeal dailyMeal_t;
typedef struct log log_t;

jmp_buf buf;

Servo foodValve;

const uint8_t SERVO_PIN = 16;
const uint8_t ALIM_SERVO_PIN = 23;
const uint8_t ERROR_LED_PIN = 25;

const uint8_t OPEN_ANGLE = 16;
const uint8_t CLOSE_ANGLE = 35;

const char* SSID = "FIZZ84084";
const char* PSWD = "Colocation21*";

IPAddress local_IP(192, 168, 0, 184);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(1, 1, 1, 1);

AsyncWebServer server(80);

void signalReady() {
  
  Serial.println("ready");
  if (digitalRead(ERROR_LED_PIN) == HIGH) {
    digitalWrite(ERROR_LED_PIN, LOW);
    delay(3000);
    digitalWrite(ERROR_LED_PIN, HIGH);
  } else {
    digitalWrite(ERROR_LED_PIN, HIGH);
    delay(3000);
    digitalWrite(ERROR_LED_PIN, LOW);
  }
}

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
  resetRetry();
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(" ");
    resetIfRetryExceeded();
  }
  Serial.print("\n");
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  } else {
    Serial.println(WiFi.localIP());
  }
  
  resetRetry();
  struct tm currentTime;
  while(!adjustRTC(currentTime)) {
    delay(5000);
    resetIfRetryExceeded();
  }

  loadMealFromFlash(currentTime);
  
  server.on("/api/meal", HTTP_POST, [] (AsyncWebServerRequest * request) {}, NULL, handlePostMeal);
  server.on("/api/meal", HTTP_DELETE, handleDeleteMeal);
  server.on("/api/meal", HTTP_GET, handleGetMeal);

  server.on("/api/log/error", HTTP_GET, handleGetErrorLog);
  server.on("/api/log/action", HTTP_GET, handleGetActionLog);

  server.begin();
  signalReady();
}

#define FAIL_GET_TIME_DELAY 300000 // 5 mins
void loop() {

  struct tm currentTime;
  if(setCurrentTime(currentTime)) { 
    if (adjustRTCIfNeeded(currentTime)) {
      feedIfMealDue(currentTime);
      delay(1000);
    } else {
      delay(FAIL_GET_TIME_DELAY);
    }
  } else {
    delay(FAIL_GET_TIME_DELAY);
  }
}