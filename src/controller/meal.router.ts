
import { Console } from "console";
import express, { Request, Response } from "express";
import { createMeal, getDueMeal, mealFed } from "../service/meal.service";
import { logReqError as logReqError } from "../util/utilities";
import { unknownError } from "./router";

export const ROUTE_MEAL = "/meal";
export const foodRouter = express.Router();

const NO_BODY_MESSAGE = "no body provided";
const NO_DATE_MESSAGE = "no date in provided body";
const BAD_DATE_MESSAGE = `bad date format, use ${new Date(1)}`;


foodRouter.get("/", async (req:Request, res: Response) => {
  let resBody = undefined;
  try {
    const meal = await getDueMeal();
    if (typeof meal === "undefined") {
      res.status(204)
    } else {
      res.status(200)
      resBody= meal;
    }
  } catch (error) {
    unknownError(req, res, error, ROUTE_MEAL);
  }
  res.send(resBody);
});

foodRouter.post("/", async (req: Request, res: Response) => {
  res.status(400);
  let resMessage = "";
  if (typeof req.body === "undefined" ) {
    resMessage = NO_BODY_MESSAGE;
    logReqError(req, ROUTE_MEAL);
  } else {
    if (typeof req.body.schedule === "undefined") {
      resMessage = NO_DATE_MESSAGE;
      logReqError(req, ROUTE_MEAL);
    } else {
      const reqSchedule = new Date(req.body.schedule);
      if (isNaN(reqSchedule.getTime())) {
        resMessage = BAD_DATE_MESSAGE;
        logReqError(req, ROUTE_MEAL);
      } else {
        try {
          createMeal(reqSchedule);
          res.status(200)
        } catch (error) {
          unknownError(req, res, error, ROUTE_MEAL);
        }
        
      }
    }
  } 
  res.send(resMessage);
});