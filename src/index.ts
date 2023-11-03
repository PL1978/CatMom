import * as dotenv from "dotenv";
import cors from "cors";
import express from "express";
import helmet from "helmet";
import { mealRouter, ROUTE_MEAL } from "./controller/meal.router";
import { mealDueRouter, ROUTE_MEAL_DUE } from "./controller/dueMeal.router";
import { mealFedRouter, ROUTE_MEAL_FED } from "./controller/fedMeal.router";


dotenv.config();

if (!process.env.PORT) {
  process.exit(1);
}

const ROUTE_ROOT = "/api";

const PORT: number = parseInt(process.env.PORT as string, 10);
const app = express();

app.use(helmet());
app.use(cors());
app.use(express.json());
app.use(`${ROUTE_ROOT}${ROUTE_MEAL_DUE}`, mealDueRouter);
app.use(`${ROUTE_ROOT}${ROUTE_MEAL_FED}`, mealFedRouter);
app.use(`${ROUTE_ROOT}${ROUTE_MEAL}`, mealRouter);

app.listen(PORT, () => {
  console.log(`Started on port ${PORT}`);
});