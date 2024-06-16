const AWS = require('aws-sdk');

const connectToDynamoDB = async () => {
    AWS.config.update({
        accessKeyId: process.env.AWS_ACCESS_KEY_ID,
        secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY,
        region: process.env.AWS_REGION
    });

    const dynamodb = new AWS.DynamoDB();

    const tableParams = {
        TableName: 'Devices',
        KeySchema: [
            { AttributeName: 'deviceId', KeyType: 'HASH' }
        ],
        AttributeDefinitions: [
            { AttributeName: 'deviceId', AttributeType: 'S' }
        ],
        ProvisionedThroughput: {
            ReadCapacityUnits: 5,
            WriteCapacityUnits: 5
        }
    };

    try {
        await dynamodb.describeTable({ TableName: 'Devices' }).promise();
        console.log('Table "Devices" already exists.');
    } catch (err) {
        if (err.code === 'ResourceNotFoundException') {
            await dynamodb.createTable(tableParams).promise();
            console.log('Table "Devices" created successfully.');
        } else {
            console.error('Error describing table "Devices":', JSON.stringify(err, null, 2));
        }
    }
};

module.exports = { connectToDynamoDB };
