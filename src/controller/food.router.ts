import express, { Request, Response } from "express";

export const ROUTE_FOOD = "/food";
export const foodRouter = express.Router();


foodRouter.get("/", async (req: Request, res: Response) => {
  res.status(200).send({"message" :"nouriture"});
});