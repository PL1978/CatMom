import { MealTime } from "../model/MealTime";
import { getAllWithValueSmallerAndValueDifferent, updateAttrValueWithID} from "./dao/food.dao";


export async function getCurrentMeal() : Promise<MealTime | undefined> {
    const currentDate = new Date();
    const currentMinuteOfDay = currentDate.getHours() * 60 + currentDate.getMinutes();
    return await getAllWithValueSmallerAndValueDifferent("schedule", currentMinuteOfDay, "last", currentDate.getDay());
}

export async function mealFed(id: number): Promise<number|undefined> {
    const currentDate = new Date();
    return await updateAttrValueWithID(id, "last", currentDate.getDay());
}