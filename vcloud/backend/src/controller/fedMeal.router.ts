
import express, { Request, Response } from "express";
import { mealFed } from "../service/meal.service";
import { logReqError } from "../util/utilities";
import { ROUTE_MEAL } from "./meal.router";
import { unknownError } from "./router";

export const ROUTE_MEAL_FED = `${ROUTE_MEAL}/fed`;
export const mealFedRouter = express.Router();

const NO_ID_MESSAGE =  "request without id";
const BAD_ID_MESSAGE = "bad id format, use numerical value";

mealFedRouter.post("/", async (req: Request, res: Response) => {
  res.status(400);
  let resMessage = "";
  if (typeof req.body.id === "undefined") {
    console.error(NO_ID_MESSAGE);
    resMessage = NO_ID_MESSAGE;
    logReqError(req, ROUTE_MEAL_FED);
  } else {
    const id = parseInt(req.body.id);
    if (isNaN(id)) {
      console.error(BAD_ID_MESSAGE);
      resMessage = BAD_ID_MESSAGE;
      logReqError(req, ROUTE_MEAL_FED);

    } else {
      try {
        const updatedItemID = await mealFed(id);
        if (typeof updatedItemID === "undefined") {
          res.status(204);
        } else {
          res.status(200);
        }
      } catch (error) {
        unknownError(req, res, error, ROUTE_MEAL_FED);
      }
    }
  }
  res.send(resMessage);
});