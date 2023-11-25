#include <ESPping.h>
#include "time.h"

#define DATE_LENGTH 20
const char GET_TIME_ERROR[DATE_LENGTH] = "ERROR_GETTING_TIME!";

uint8_t lastRTCAdjustment = 40;
const char* ESTERN_TZ = "EST5EDT,M3.2.0,M11.1.0";
const char* NTP_SERVER = "pool.ntp.org";

bool setCurrentTime(struct tm& toSet) {
  bool isSet = getLocalTime(&toSet);
  if(!isSet){
      signalError("TIME");
  }
  return isSet;
}

bool adjustRTC(struct tm& currentTime, bool isTimeInitialized) {
  // Essayez de vous connecter au serveur NTP
  bool timeIsSet = isTimeInitialized;
  if (!Ping.ping(NTP_SERVER)) {
    signalError("ANTP");
  } else {

    configTime(0, 0, NTP_SERVER);
    setenv("TZ", ESTERN_TZ, 1);
    tzset();

    if (isTimeInitialized) {
      lastRTCAdjustment = static_cast<uint8_t>(currentTime.tm_mon);
    } else if(setCurrentTime(currentTime)) {
      lastRTCAdjustment = static_cast<uint8_t>(currentTime.tm_mon);
      isTimeInitialized =true;
    }
  }
  return timeIsSet;
}

void adjustRTCIfNeeded(struct tm& currentTime) {
  if (lastRTCAdjustment != currentTime.tm_mon) {
    adjustRTC(currentTime, true);
  }
}

void setTimestampString(char toSet[DATE_LENGTH]) {
  struct tm currentTime;

  if(getLocalTime(&currentTime)){
    strftime(toSet, sizeof(char)*DATE_LENGTH, "%Y-%m-%dT%H:%M:%S", &currentTime);
  } else {
    strncpy(toSet, GET_TIME_ERROR, sizeof(char)*DATE_LENGTH);
  }
}