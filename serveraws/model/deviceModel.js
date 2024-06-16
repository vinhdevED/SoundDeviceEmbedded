require('dotenv').config();
const AWS = require('aws-sdk');
const docClient = new AWS.DynamoDB.DocumentClient();

AWS.config.update({
    accessKeyId: process.env.AWS_ACCESS_KEY_ID,
    secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY,
    region: process.env.AWS_REGION
});

const addDevice = async (device) => {
    const params = {
        TableName: 'Devices',
        Item: device
    };

    try {
        await docClient.put(params).promise();
        return { success: true, message: 'Device added successfully.' };
    } catch (err) {
        console.error('Unable to add device:', JSON.stringify(err, null, 2));
        return { success: false, message: 'Error adding device.' };
    }
};

const getDevice = async (deviceId) => {
    const params = {
        TableName: 'Devices',
        Key: { deviceId }
    };

    try {
        const data = await docClient.get(params).promise();
        return data.Item;
    } catch (err) {
        console.error('Unable to get device:', JSON.stringify(err, null, 2));
        return null;
    }
};

const getAllDevices = async () => {
    const params = {
        TableName: 'Devices'
    };

    try {
        const data = await docClient.scan(params).promise();
        return data.Items;
    } catch (err) {
        console.error('Unable to scan devices:', JSON.stringify(err, null, 2));
        return [];
    }
};


module.exports = {
    addDevice,
    getDevice,
    getAllDevices
};
