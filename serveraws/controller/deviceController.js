const { addDevice, getDevice ,getAllDevices} = require('../model/deviceModel');
const AWS = require('aws-sdk');
const dynamodb = new AWS.DynamoDB.DocumentClient();

const fetchAllDevice = async (req, res) => {
    try {
        const devices = await getAllDevices();
        res.status(200).json(devices);
    } catch (error) {
        console.error('Error fetching devices:', error);
        res.status(500).json({ message: 'Failed to fetch devices' });
    }
};

const connectDeviceWaiting = async (req, res) => {
    const {deviceId } = req.body;

    try {

        // Update device status in DynamoDB
        const params = {
            TableName: 'Devices', // Replace with your DynamoDB table name
            Key: { deviceId: deviceId },
            UpdateExpression: 'set #status = :status',
            ExpressionAttributeNames: {
                '#status': 'status'
            },
            ExpressionAttributeValues: {
                ':status': true // Update status to true (connected)
            },
            ReturnValues: 'UPDATED_NEW'
        };

        const updatedDevice = await dynamodb.update(params).promise();

        res.status(200).json({ message: 'Device connected successfully', updatedDevice });
        // const device = await getDevice(deviceId); // Sử dụng hàm từ deviceModel để lấy thiết bị từ cơ sở dữ liệu
        //
        // if (!device) {
        //     return res.status(404).json({ error: 'Device not found' });
        // }
        //
        // // Thực hiện các xử lý kết nối ở đây (nếu cần)
        // device.status = 'connecting'; // Chuyển trạng thái thiết bị thành connecting
        //
        // // Giả định kết nối thành công sau 2 giây
        // setTimeout(() => {
        //     device.status = 'connected';
        //     res.json({ message: 'Device connected successfully' });
        // }, 2000);
    } catch (error) {
        console.error('Error connecting device:', error);
        res.status(500).json({ message: 'Failed to connect device' });
    }
}


module.exports = {
    fetchAllDevice,
    connectDeviceWaiting
};
