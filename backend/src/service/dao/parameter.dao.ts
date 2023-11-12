import { ParameterItem, TABLE_NAME } from "../../model/item/parameter.item";
import { createClient, getItem } from "./dynamoDB.dao";

const docClient = createClient();

export async function getParameter(id:number): Promise<undefined|ParameterItem> {
    const params = await getItem(TABLE_NAME, id, docClient);
    return typeof params.Item === "undefined" ? undefined : params.Item as ParameterItem;
}