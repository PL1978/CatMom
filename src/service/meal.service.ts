import { ParameterItem, TABLE_ID } from "../model/item/parameter.item";
import { getFirstWithValueSmallerAndValueDifferent, updateAttrValueWithID} from "./dao/meal.dao";
import { getParameter } from "./dao/parameter.dao";


export async function getDueMeal() : Promise<ParameterItem | undefined> {
    const currentDate = new Date();
    const currentMinuteOfDay = currentDate.getHours() * 60 + currentDate.getMinutes();
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