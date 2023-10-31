
import express, { Request, Response } from "express";
import { getCurrentMeal, mealFed } from "../service/meal.service";
import { logErrorReq as logReqError } from "../util/utilities";

export const ROUTE_FOOD = "/meal";
export const foodRouter = express.Router();


foodRouter.get("/", async (res: Response) => {
  const meal = await getCurrentMeal();
  let resBody: {[key:string]:any} = {};
  if (typeof meal === "undefined") {
    res.status(204)
  } else {
    res.status(200)
    resBody.id = meal.id;
  }
  res.send(resBody);
});

foodRouter.post("/", async (req: Request, res: Response) => {
  res.status(400);
  if (typeof req.body.id === "undefined") {
    console.error("request without id");
    logReqError(req, ROUTE_FOOD);

  } else {
    const id = parseInt(req.body.id);
    if (isNaN(id)) {
      console.error("bad id format, use numerical value");
      logReqError(req, ROUTE_FOOD);

    } else {
      try {
        const updatedItemID = await mealFed(id);
        if (typeof updatedItemID === "undefined") {
          res.status(204);
        } else {
          res.status(200);
        }
      } catch (error) {
        res.status(500);
        console.error(`error while posting request :\n${error}`);
        logReqError(req, ROUTE_FOOD);
      }
    }
  }
  res.send();
});