
import express, { Request, Response } from "express";
import { getDueMeal,  } from "../service/meal.service";
import { ROUTE_MEAL } from "./meal.router";
import { unknownError } from "./router";

export const ROUTE_MEAL_DUE = `${ROUTE_MEAL}/due`;
export const mealDueRouter = express.Router();


mealDueRouter.get("/", async (req:Request, res: Response) => {
  try {
    const meal = await getDueMeal();
    res.status(typeof meal === "undefined" ? 204 : 200);
    res.send(meal);
  } catch (error) {
    unknownError(req, res, error, ROUTE_MEAL_DUE);
  }
  
});