uint8_t errorCount = 0;
log_t loggedErrors[LOG_COUNT_MAX];
const char* errorArrayKey = "errors";
const char* errorItemKey = "error";

void signalError(char error[LOG_CODE_LENGTH]) {
  log(error, loggedErrors, errorCount);
  Serial.println(error);
  digitalWrite(ERROR_LED_PIN, HIGH);
}

void clearError() {
  digitalWrite(ERROR_LED_PIN, LOW);
}

void handleGetErrorLog(AsyncWebServerRequest *request) {
    handleGetLog(request, loggedErrors, errorCount, errorArrayKey, errorItemKey);
    clearError();
}