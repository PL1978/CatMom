import { DynamoDBClient, ScanCommand } from "@aws-sdk/client-dynamodb";
import { DynamoDBDocumentClient, GetCommand, UpdateCommand } from "@aws-sdk/lib-dynamodb";
import { unmarshall } from "@aws-sdk/util-dynamodb";
import { MealTime, TABLE_NAME, } from "../../model/MealTime";


const client = new DynamoDBClient({region:"us-east-2"});
const docClient = DynamoDBDocumentClient.from(client);

export async function getAllWithValueSmallerAndValueDifferent(smallerAttr: string, smallerValue:number, difAttr: string, diffValue: number): Promise<MealTime | undefined> {
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
      
      return typeof response.Items === "undefined" || typeof response.Items!.at(0) === "undefined" ? undefined : unmarshall(response.Items.at(0)!) as MealTime;
}

export async function updateAttrValueWithID(id: number, attr: string, value: number) : Promise<number|undefined> {
  const getCommand = new GetCommand({
    TableName: TABLE_NAME,
    Key: {id: id}
  });
  const existingItem = await client.send(getCommand);
  let itemUpdatedID = undefined;
  if (typeof existingItem.Item !== "undefined") {
    const updateCommand = new UpdateCommand({
      TableName: TABLE_NAME,
      Key: {id: id},
      UpdateExpression: 'SET #attrName = :attrValue',
      ExpressionAttributeNames: { '#attrName': attr },
      ExpressionAttributeValues: { ':attrValue': value.toString() },
    });
    client.send(updateCommand);
    itemUpdatedID = id;
  }

  return itemUpdatedID;
}