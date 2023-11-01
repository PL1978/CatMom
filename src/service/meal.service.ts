import { MealTimeItem } from "../model/item/mealTime.item";
import { ParameterItem, TABLE_ID } from "../model/item/parameter.item";
import { addMeal, getFirstWithValueSmallerAndValueDifferent, updateAttrValueWithID} from "./dao/meal.dao";
import { getParameter } from "./dao/parameter.dao";

function dateToNumber(date:Date): number {
    return date.getHours() * 60 + date.getMinutes();
}

export function createMeal(feedTime: Date) {
    const meal: MealTimeItem = {
        id : new Date().getTime(),
        last : -1,
        schedule : dateToNumber(feedTime)
    }
    addMeal(meal);
}

export async function getDueMeal() : Promise<ParameterItem | undefined> {
    const currentDate = new Date();
    const currentMinuteOfDay = dateToNumber(currentDate);
    const dueMeal = await getFirstWithValueSmallerAndValueDifferent("schedule", currentMinuteOfDay, "last", currentDate.getDay());
    let formatedDueMeal = undefined;
    if (typeof dueMeal !== "undefined") {
        let params = await getParameter(TABLE_ID);
        if (typeof params === "undefined") {
            throw new Error("No params in db");
        }
        formatedDueMeal = params;
        formatedDueMeal.id = dueMeal.id;
    }
    return formatedDueMeal;
}

export async function mealFed(id: number): Promise<number|undefined> {
    const currentDate = new Date();
    return await updateAttrValueWithID(id, "last", currentDate.getDay());
}