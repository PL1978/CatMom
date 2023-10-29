import express, { Request, Response } from "express";
import { getCurrentMeal } from "../service/meal.service";

export const ROUTE_FOOD = "/food";
export const foodRouter = express.Router();


foodRouter.get("/", async (req: Request, res: Response) => {
  await getCurrentMeal();
  res.status(200).send({"message" :"nouriture"});
});