
uint8_t actionCount = 0;
log_t loggedActions[LOG_COUNT_MAX];

void logAction(char action[LOG_CODE_LENGTH]) {
  log(action, loggedActions, actionCount);
}

void handleGetActionLog(AsyncWebServerRequest *request) {
    if (actionCount > 0) {
      DynamicJsonDocument errorLogJsonDocument(1024);
      JsonArray errorArray = errorLogJsonDocument.createNestedArray("errors");
      for (size_t i = 0; i < actionCount; i++) {
          JsonObject errorObject = errorArray.createNestedObject();
          errorObject["action"] = loggedActions[i].log;
          errorObject["time"] = loggedActions[i].time;
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

