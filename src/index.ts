import * as dotenv from "dotenv";
import cors from "cors";
import express from "express";
import helmet from "helmet";
import { foodRouter, ROUTE_FOOD } from "./controller/food.router";


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
app.use(`${ROUTE_ROOT}${ROUTE_FOOD}`, foodRouter);

app.listen(PORT, () => {
  console.log(`Listening on port ${PORT}`);
});