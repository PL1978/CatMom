#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <setjmp.h>
#include <stdio.h>

jmp_buf buf;

char* concat(const char *s1, const char *s2) {
    char *result = (char*)malloc(strlen(s1) + strlen(s2) + 1); // +1 pour le caractère nul de fin de chaîne
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
const char* ssid = "FIZZ84084";
const char* password = "Colocation21*";

const char* MEAL_API_URI_ROOT = "http://192.168.0.14:8080/api/meal";
const char* DUE_MEAL_URI = concat(MEAL_API_URI_ROOT, "/due");
const char* FED_MEAL_URI = concat(MEAL_API_URI_ROOT, "/fed");
const ushort DUE_MEAL_HTTP_STATUS = 200;
const ushort NO_DUE_MEAL_HTTP_STATUS = 204;


typedef struct gotMeal {
  bool isMealDue;
  uint id;
  ushort openAngle;
  ushort closeAngle;
  uint openTime;
} gotMeal_t;

gotMeal_t* getDueMeal();

Servo foodValve;

const ushort SERVO_PIN = 16;
const ushort ALIM_SERVO_PIN = 23;
const ushort ERROR_LED_PIN = 25;

void signalError() {
  digitalWrite(ERROR_LED_PIN, HIGH);
}

void clearError() {
  digitalWrite(ERROR_LED_PIN, LOW);
}

JsonVariant getJsonValue(const char* key, DynamicJsonDocument document) {
  if (document.containsKey(key)) {
    return document[key];
  } else {
    Serial.print("unexpected body missing key ");
    Serial.println(key);
    signalError();
    longjmp(buf, 1);
  }
}

void feed(ushort openAngle, ushort closeAngle, uint openTime) {
  digitalWrite(ALIM_SERVO_PIN, HIGH);
  foodValve.write(openAngle);
  delay(openTime);
  foodValve.write(closeAngle);
  delay(200);

  digitalWrite(ALIM_SERVO_PIN, LOW);
}

void confirmFeed(uint id) {
  DynamicJsonDocument jsonRequestDoc(16);
  jsonRequestDoc["id"] = id;
  const size_t bodySize = measureJson(jsonRequestDoc) + 1;
  char* serializedBody = (char*)malloc(bodySize);

  if (serializedBody == NULL) {
    Serial.print("Unable to allow memory for body of meal ");
    Serial.println(id);
    signalError();
  } else {
    serializeJson(jsonRequestDoc, serializedBody, bodySize);

    HTTPClient http;
    http.begin(FED_MEAL_URI);
    http.addHeader("Content-Type", "application/json");
    bool responseInvalid = false;
    do {
      int httpResponseCode = http.POST(serializedBody);
      responseInvalid = httpResponseCode < 0 || httpResponseCode >= 300;
      if (responseInvalid) {
        Serial.print("Unable to confirm meal with id ");
        Serial.print(id);
        Serial.print(", Error code is ");
        Serial.println(httpResponseCode);
        signalError();
        //delay(1800000);
        delay(3000);
      } else {
        Serial.print("Meal fed posted with body ");
        Serial.println(serializedBody);
        http.end();
      }
    } while(responseInvalid);
  }
  free(serializedBody);
  jsonRequestDoc.clear();
}

gotMeal_t* getDueMeal() {
  gotMeal_t* dueMeal = (gotMeal_t*)malloc(sizeof(gotMeal_t));
  dueMeal->isMealDue = false;

  HTTPClient http;
  http.begin(DUE_MEAL_URI);
  int httpResponseCode = http.GET();

  DynamicJsonDocument jsonResponseBody(128);
  
  if (!setjmp(buf)) {
    if (httpResponseCode >= 0) {
      if (httpResponseCode == DUE_MEAL_HTTP_STATUS) {
          DeserializationError error = deserializeJson(jsonResponseBody, http.getString());
          if (error) {
            signalError();
            Serial.print("Error while deserializing ");
            Serial.println(error.c_str());
          } else {
            
            dueMeal->id = getJsonValue("id", jsonResponseBody).as<uint>();
            dueMeal->openAngle = getJsonValue("openAngle", jsonResponseBody).as<ushort>();
            dueMeal->closeAngle = getJsonValue("closeAngle", jsonResponseBody).as<ushort>();
            dueMeal->openTime = getJsonValue("openTimeMS", jsonResponseBody).as<uint>();
            dueMeal->isMealDue = true;
            Serial.print("Successfully got meal id ");
            Serial.println(dueMeal->id);
            clearError();
          }
        } else if(httpResponseCode == NO_DUE_MEAL_HTTP_STATUS) {
          Serial.println("No meal due");
          clearError();
        } else {
          signalError();
          Serial.print("Unexpected http stauts ");
          Serial.println(httpResponseCode);
      }
    } else {
      signalError();
      Serial.print("Error code during fetch ");
      Serial.println(httpResponseCode);
    }
  }
  
  http.end();
  jsonResponseBody.clear();

  return dueMeal;
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(" ");
  }
  Serial.println("connected");
  
  pinMode(ALIM_SERVO_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  foodValve.attach(SERVO_PIN);
}

void loop() {
  
  gotMeal_t* dueMeal = getDueMeal();
  Serial.println(dueMeal->isMealDue);
  if (dueMeal->isMealDue) {
    feed(dueMeal->openAngle, dueMeal->closeAngle, dueMeal->openTime);
    confirmFeed(dueMeal->id);
  }
  free(dueMeal);
  delay(3000);
  
}
