import { ScanCommand } from "@aws-sdk/client-dynamodb";
import { DeleteCommand, PutCommand, UpdateCommand } from "@aws-sdk/lib-dynamodb";
import { unmarshall } from "@aws-sdk/util-dynamodb";
import { LookupAllOptions } from "dns";
import { MealTimeItem, TABLE_NAME, } from "../../model/item/mealTime.item";
import { createClient, getItem } from "./dynamoDB.dao";

const docClient = createClient();

export function add(toAdd: MealTimeItem) {
  const addCommand = new PutCommand({
    TableName: TABLE_NAME,
    Item: toAdd
  });
  docClient.send(addCommand);
}

export async function getAll(): Promise<Array<MealTimeItem>> {
  const getCommand = new ScanCommand({
    TableName: TABLE_NAME
  });
  const all = await docClient.send(getCommand);
  let allMeals: Array<MealTimeItem>;
  if (typeof all.Items === "undefined") {
    allMeals = new Array();
   } else {
     allMeals = all.Items.map((mealItem: Record<string, any>) => unmarshall(mealItem) as MealTimeItem);
   }
  return allMeals;
}

export async function getFirstWithValueSmallerAndValueDifferent(smallerAttr: string, smallerValue:number, difAttr: string, diffValue: number): Promise<MealTimeItem | undefined> {
    const scanCommand = new ScanCommand({
        TableName: TABLE_NAME,
        FilterExpression: `#smallerAttr <= :smallerValue AND #difAttr <> :diffValue`,
        ExpressionAttributeNames: {
            '#smallerAttr': smallerAttr,
            '#difAttr': difAttr,
          },
          ExpressionAttributeValues: {
            ':smallerValue': { N: smallerValue.toString()},
            ':diffValue': { N: diffValue.toString()},
          }
      });
    
      const response = await docClient.send(scanCommand);
      return typeof response.Items === "undefined" || typeof response.Items!.at(0) === "undefined" ? undefined : unmarshall(response.Items.at(0)!) as MealTimeItem;
}

export async function updateAttrValueWithID(id: number, attr: string, value: number) : Promise<number|undefined> {
  const existingItem = await getItem(TABLE_NAME, id, docClient);

  let itemUpdatedID = undefined;
  if (typeof existingItem.Item !== "undefined") {
    const updateCommand = new UpdateCommand({
      TableName: TABLE_NAME,
      Key: {id: id},
      UpdateExpression: 'SET #attrName = :attrValue',
      ExpressionAttributeNames: { '#attrName': attr },
      ExpressionAttributeValues: { ':attrValue': value },
    });
    docClient.send(updateCommand);
    itemUpdatedID = id;
  }
  return itemUpdatedID;
}

export function remove(id :number) {
    const deleteCommand = new DeleteCommand({
      TableName: TABLE_NAME,
      Key: {
        id: id
      }
    });
    docClient.send(deleteCommand);
}