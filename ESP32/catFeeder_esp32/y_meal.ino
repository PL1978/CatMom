const char* MEAL_HOUR_KEY = "mealHour";
const char* OPEN_TIME_KEY = "openTime";
const uint8_t MEAL_BODY_SIZE = 64;

struct scheduledMeal {
  uint8_t mealHour;
  ushort openTime;
};

struct dailyMeal {
  scheduledMeal_t meal;
  uint8_t lastServedDay;
};

size_t mealCount;
dailyMeal_t* meals;

void loadMealFromFlash(struct tm& currentTime) {
  loadMealFromFlash(currentTime, &meals, mealCount);
}

bool isMealHourDue(scheduledMeal_t& toCheckIfDue, struct tm& currentTime) {
  return currentTime.tm_hour >= toCheckIfDue.mealHour;
}

bool isMealDue(dailyMeal_t& toCheckIfDue, struct tm& currentTime) {
  return toCheckIfDue.lastServedDay != currentTime.tm_mday && isMealHourDue(toCheckIfDue.meal, currentTime);
}

/**
* If the meal is due, we consider it served. This way, meals are not served when they are created or on restart.
*/
uint8_t makeLastServed(scheduledMeal_t& meal, struct tm& currentTime) {
  return isMealHourDue(meal, currentTime) ? static_cast<uint8_t>(currentTime.tm_mday) : 40; // some impossible value since max m_day = 31
}

void addMeal(scheduledMeal_t& toAdd, struct tm& currentTime) {
  dailyMeal_t* withAddedMeal = new dailyMeal_t[mealCount + 1];
  if (mealCount > 0) {
    memcpy(withAddedMeal, meals, mealCount * sizeof(dailyMeal_t));
  }
  dailyMeal_t dailyToAdd;
  dailyToAdd.meal = toAdd;
  dailyToAdd.lastServedDay = makeLastServed(toAdd, currentTime);
  withAddedMeal[mealCount] = dailyToAdd;

  delete[] meals;
  meals = withAddedMeal;
  mealCount++;

  saveMealsToFlash(meals, mealCount);
}

bool removeMeal(uint8_t mealHour) {
  bool isRemoved = false;
  for (size_t i = 0; i<mealCount; i++) {
    if (meals[i].meal.mealHour == mealHour) {
      memmove(meals + i, meals + i + 1, (mealCount - i - 1) * sizeof(dailyMeal_t));
      mealCount--;
      isRemoved = true;
      saveMealsToFlash(meals, mealCount);
      break;
    }
  }
  return isRemoved;
}

void feedIfMealDue(struct tm& currentTime) {
  for (size_t i= 0; i<mealCount; i++) {
    if (isMealDue(meals[i], currentTime)) {
      feed(meals[i].meal.openTime);
      meals[i].lastServedDay = currentTime.tm_mday;
      logAction("FEED");
      break;
    }
  }
}

void handlePostMeal(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  StaticJsonDocument<MEAL_BODY_SIZE> mealBody;
  std::string body(reinterpret_cast<char*>(data), len);
  scheduledMeal_t postedMeal;

  if (setjmp(buf)) {
    request->send(400, "text/plain", (std::string("Missing field ") + failedBodyField).c_str());
  } else {
    DeserializationError error = deserializeJson(mealBody, body);
    if (error) {
      request->send(500, "text/plain", (std::string("Error while parsing json body: ") + error.c_str()).c_str());
    } else {
      postedMeal.openTime = getJsonValue(OPEN_TIME_KEY, mealBody).as<ushort>();
      if (!mealBody.containsKey(MEAL_HOUR_KEY)) {
        feed(postedMeal.openTime);
        request->send(200); 
      } else {
        postedMeal.mealHour = getJsonValue(MEAL_HOUR_KEY, mealBody).as<uint8_t>();
        if (postedMeal.mealHour > 23) {
          request->send(400, "text/plain", "provide a meal hour between 0 and 23");
        } else {
          bool isHourConflict = false;
          for (size_t i =0; i<mealCount; i++) {
            isHourConflict |= meals[i].meal.mealHour == postedMeal.mealHour;
          }
          if (isHourConflict) {
            request->send(409, "text/plain", "Conflict with existing scheduled meal");
          } else {
            struct tm currentTime;
            if(setCurrentTime(currentTime)){
              addMeal(postedMeal, currentTime);
              request->send(201); 
            } else {
              request->send(500, "text/plain", "Error while getting local time: ");
            }         
          }
        }
      }
    }
  }
  mealBody.clear();
}

void handleDeleteMeal(AsyncWebServerRequest *request) {
  if (request->hasParam(MEAL_HOUR_KEY)) {
    AsyncWebParameter* mealHourParam = request->getParam(MEAL_HOUR_KEY);
    uint8_t mealHour = atoi(mealHourParam->value().c_str());
    if (removeMeal(mealHour)) {
      request->send(200);
    } else {
      request->send(204);
    }
  } else {
    request->send(400, "text/plain", "Missing mealHour querry");
  }
}

void handleGetMeal(AsyncWebServerRequest *request) {
  if (mealCount > 0) {
    DynamicJsonDocument mealsJsonDocument(MEAL_BODY_SIZE*mealCount);
    JsonArray mealsArray = mealsJsonDocument.createNestedArray("meals");
    for (size_t i = 0; i < mealCount; i++) {
        JsonObject mealObject = mealsArray.createNestedObject();
        mealObject[MEAL_HOUR_KEY] = meals[i].meal.mealHour;
        mealObject[OPEN_TIME_KEY] = meals[i].meal.openTime;
    }
    String mealsJson;
    serializeJson(mealsJsonDocument, mealsJson);
    request->send(200, "application/json", mealsJson);
    mealsJsonDocument.clear();
  } else {
    request->send(204);
  }
}