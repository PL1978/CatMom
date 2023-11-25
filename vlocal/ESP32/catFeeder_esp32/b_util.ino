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
