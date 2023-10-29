import { DynamoDBClient, ScanCommand } from "@aws-sdk/client-dynamodb";
import { DynamoDBDocumentClient, GetCommand } from "@aws-sdk/lib-dynamodb";
import { FeedTime, TABLE_NAME } from "../../model/FeedTime";


const client = new DynamoDBClient({region:"us-east-2"});
const docClient = DynamoDBDocumentClient.from(client);

export async function getAllWithValueSmallerAndValueDifferent(smallerAttr: string, smallerValue:number, difAttr: string, diffValue: number) {
    const command = new ScanCommand({
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
    
      const response = await docClient.send(command);
      console.log(response.Items?.at(0)?.FeedTime);
}