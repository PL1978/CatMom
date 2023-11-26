#include <Preferences.h>

Preferences flash;

const char* FS_GROUP = "CATMOM";
const char* FS_MEAL_NB = "mealCount";
const char* FS_MEALS = "meals";

void saveMealsToFlash(dailyMeal_t* meals, size_t mealCount) {
  if (mealCount > 0) {
    flash.begin(FS_GROUP, false);
    flash.putBytes(FS_MEAL_NB, &mealCount, sizeof(mealCount));

    scheduledMeal_t scheduledMeals_[mealCount];
    for (size_t i=0; i<mealCount; i++) {
      scheduledMeals_[i] = meals[i].meal;
    }

    flash.putBytes(FS_MEALS, &scheduledMeals_, mealCount * sizeof(scheduledMeal_t));
    
    flash.end();
  }
}

void loadMealFromFlash(struct tm& currentTime, dailyMeal_t** mealsToLoadInto, size_t& mealCount) {
    flash.begin(FS_GROUP, true);
    size_t mealCount_;

    if (flash.isKey(FS_MEAL_NB)) {
      flash.getBytes(FS_MEAL_NB, &mealCount_, sizeof(mealCount_));
    } else {
      mealCount_ = 0;
    }

    if (mealCount_ > 0) {
      if (flash.isKey(FS_MEAL_NB)) {
        *mealsToLoadInto = new dailyMeal_t[mealCount_];
        scheduledMeal_t scheduledMeals_[mealCount_];
        flash.getBytes(FS_MEALS, scheduledMeals_, mealCount_ * sizeof(scheduledMeal_t));

        for (size_t i =0; i<mealCount_; i++) {
          (*mealsToLoadInto)[i].meal = scheduledMeals_[i];
          (*mealsToLoadInto)[i].lastServedDay = makeLastServed(scheduledMeals_[i], currentTime);
        }
        
      } else {
        mealCount_ = 0;
      }
    }
    mealCount = mealCount_;
    flash.end();
}