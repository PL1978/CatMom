import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { DynamoDBDocumentClient, GetCommand, GetCommandOutput } from "@aws-sdk/lib-dynamodb";

const region = "us-east-2";

export function createClient(): DynamoDBDocumentClient {
    const client = new DynamoDBClient({region:region});
    return DynamoDBDocumentClient.from(client);
}

export function getItem(tableName:string, id:number, client:DynamoDBDocumentClient) : Promise<GetCommandOutput>{
    const getCommand = new GetCommand({
        TableName: tableName,
        Key: {id: id}
      });
    return client.send(getCommand);
}
