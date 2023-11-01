import { logReqError } from "../util/utilities";
import { Request, Response } from "express";

export function unknownError(req:Request, res:Response, error:unknown, path:string) {
    res.status(500);
    console.error(`error while posting request :\n${error}`);
    logReqError(req, path);
}