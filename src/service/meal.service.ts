import { getAllWithValueSmallerAndValueDifferent} from "./dao/food.dao";


export async function getCurrentMeal() {
    await getAllWithValueSmallerAndValueDifferent("schedule", 3, "last", 1);

}