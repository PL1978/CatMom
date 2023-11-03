
import express, { Request, Response } from "express";
import { createMeal, deleteMeal, getAllMeals } from "../service/meal.service";
import { logReqError as logReqError } from "../util/utilities";
import { bodyValidation, unknownError } from "./router";

export const ROUTE_MEAL = "/meal";
export const mealRouter = express.Router();



const BAD_DATE_MESSAGE = `bad date format, use ${new Date(1)}`;


mealRouter.get("/", async (req: Request, res: Response) => {
  
  try {
    res.status(200);
    res.send(await getAllMeals());
  } catch (error) {
    unknownError(req, res, error, ROUTE_MEAL);
    res.send();
  }
  
});

mealRouter.post("/", async (req: Request, res: Response) => {
  if (bodyValidation(req, res, ["schedule"], ROUTE_MEAL)) {
    const reqSchedule = new Date(req.body.schedule);
    if (isNaN(reqSchedule.getTime())) {
      res.status(400);
      res.send(BAD_DATE_MESSAGE);
      logReqError(req, ROUTE_MEAL);
    } else {
      try {
        createMeal(reqSchedule);
        res.status(200);
        res.send();
      } catch (error) {
        unknownError(req, res, error, ROUTE_MEAL);
      }
    }
  }
  
});

mealRouter.delete("/", async (req: Request, res: Response) => {
  if (bodyValidation(req, res, ["id"], ROUTE_MEAL)) {
    const id = parseInt(req.body.id);
    if (isNaN(id)) {
      res.status(400);
      res.send("Bad id, please provide a number");
    } else {
      deleteMeal(id);
      res.status(200);
      res.send();
    }
  }
});