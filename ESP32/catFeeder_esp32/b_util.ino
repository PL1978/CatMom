#define LOG_COUNT_MAX 10
#define LOG_CODE_LENGTH 5

typedef struct log {
  char log[LOG_CODE_LENGTH];
  char time[DATE_LENGTH];
};

void log(char log[LOG_CODE_LENGTH], log_t logBuf[LOG_COUNT_MAX], uint8_t& logCount) {
  if (logCount < LOG_COUNT_MAX) {
    logCount++;
  }
  if (logCount > 1) { 
    memmove(logBuf + 1, logBuf, (logCount - 1) * sizeof(log_t));
  }
  strncpy(logBuf[0].log, log, sizeof(char)*LOG_CODE_LENGTH);
  setTimestampString(logBuf[0].time);
}

void handleGetLog(AsyncWebServerRequest *request, log_t logBuf[LOG_COUNT_MAX], uint8_t count, const char* keyParentName, const char* logItemName) {
    if (count > 0) {
      DynamicJsonDocument logJsonDocument(1024);
      JsonArray logArray = logJsonDocument.createNestedArray(keyParentName);
      for (size_t i = 0; i < count; i++) {
          JsonObject logObject = logArray.createNestedObject();
          logObject[logItemName] = logBuf[i].log;
          logObject["time"] = logBuf[i].time;
      }
      String logJson;
      serializeJson(logJsonDocument, logJson);
      request->send(200, "application/json", logJson);
      logJsonDocument.clear();
    } else {
      request->send(204);
    }
}

 size_t retryCount =0;
 void resetRetry() {
  retryCount=0;
 }

 void resetIfRetryExceeded() {
    retryCount++;
    if (retryCount > 20) {
      ESP.restart();
    }
 }