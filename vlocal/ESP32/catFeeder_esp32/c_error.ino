
uint8_t errorCount = 0;
log_t loggedErrors[LOG_COUNT_MAX];

void signalError(char error[LOG_CODE_LENGTH]) {
  log(error, loggedErrors, errorCount);
  Serial.println(error);
  digitalWrite(ERROR_LED_PIN, HIGH);
}

void clearError() {
  digitalWrite(ERROR_LED_PIN, LOW);
}

void handleGetErrorLog(AsyncWebServerRequest *request) {
    if (errorCount > 0) {
      DynamicJsonDocument errorLogJsonDocument(1024);
      JsonArray errorArray = errorLogJsonDocument.createNestedArray("errors");
      for (size_t i = 0; i < errorCount; i++) {
          JsonObject errorObject = errorArray.createNestedObject();
          errorObject["error"] = loggedErrors[i].log;
          errorObject["time"] = loggedErrors[i].time;
          Serial.println(i);
      }
      String errorJson;
      serializeJson(errorLogJsonDocument, errorJson);
      request->send(200, "application/json", errorJson);
      errorLogJsonDocument.clear();
    } else {
      request->send(204);
    }
    clearError();
}

