import { logReqError } from "../util/utilities";
import { Request, Response } from "express";

const REPLACE_STRING_SYMBOL = "%s";
const NO_BODY_MESSAGE = "no body provided";
const NO_KEY_MESSAGE =`no ${REPLACE_STRING_SYMBOL} in provided body`;
const BAD_NUMBER_TYPE_MESSAGE =`wrong type for ${REPLACE_STRING_SYMBOL} please provide a number`;

export function unknownError(req:Request, res:Response, error:unknown, path:string) {
    res.status(500);
    console.error(`error while posting request :\n${error}`);
    logReqError(req, path);
    res.send("Unknown error");
}

export function bodyValidation(req: Request, res: Response, requiredBodyKeys: Array<string>, route: string): boolean {
    let isValid = true;
    if (typeof req.body === "undefined") {
        res.status(400);
        res.send(NO_BODY_MESSAGE);
        logReqError(req, route);
        isValid = false;
    } else {
        for (const requiredBodyKey of requiredBodyKeys) {
            if (!(requiredBodyKey in req.body)) {
                res.status(400);
                res.send(NO_KEY_MESSAGE.replace(REPLACE_STRING_SYMBOL, requiredBodyKey));
                logReqError(req, route);
                isValid = false;
                break;
            }
        }
    }
    return isValid;
}
