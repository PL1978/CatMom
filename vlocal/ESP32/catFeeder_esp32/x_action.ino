
uint8_t actionCount = 0;
log_t loggedActions[LOG_COUNT_MAX];
const char* actionArrayKey = "actions";
const char* actionItemKey = "action";

void logAction(char action[LOG_CODE_LENGTH]) {
  log(action, loggedActions, actionCount);
}

void handleGetActionLog(AsyncWebServerRequest *request) {
    handleGetLog(request, loggedActions, actionCount, actionArrayKey, actionItemKey);
}