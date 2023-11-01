import { Request } from "express";

export function logReqError(req : Request, path: string) {
    console.error(`${new Date()}
    ip ${req.ip}
    path ${path}${JSON.stringify(req.url)}
    query ${JSON.stringify(req.query)}
    params ${JSON.stringify(req.params)}
    body ${JSON.stringify(req.body)}
    headers ${JSON.stringify(req.headers)}`);
}