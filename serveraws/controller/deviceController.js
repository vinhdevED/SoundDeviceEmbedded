const { addDevice, getDevice ,getAllDevices} = require('../model/deviceModel');
const AWS = require('aws-sdk');
const dynamodb = new AWS.DynamoDB.DocumentClient();
const { v4: uuidv4 } = require('uuid');
const { setAllowDataDisplay,getAllowDataDisplay } = require('../deviceIOT/allowDataDisplay')



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
    const { allow } = req.body; // Nhận giá trị 'allow' từ request body
    setAllowDataDisplay(allow); // Cập nhật trạng thái allowDataDisplay
    res.json({ message: `Allow data display set to ${allow}` });


}

const getDataChart = async (req, res) => {
    try {
        const params = {
            TableName: 'soundLevel',
            KeyConditionExpression: 'deviceId = :deviceId',
            ExpressionAttributeValues: {
                ':deviceId': 'ST-ESP-SZ_1-V-AMNH'
            },
            ScanIndexForward: false, // Sắp xếp giảm dần theo sort key (timestamp)
           // Limit: 100 // Số lượng mục tối đa cần lấy
        };

        dynamodb.query(params, (err, data) => {
            if (err) {
                console.error('Lỗi khi truy vấn DynamoDB:', err);
            } else {
                //console.log('Dữ liệu mới nhất:', data.Items);
                const listDatas = data.Items[0].listDatas;

                res.status(200).json(listDatas)
            }
        });


    } catch (error) {
        console.error('Error retrieving sound levels:', error);
        res.status(500).json({ error: 'Failed to retrieve sound levels' });
    }

}

// SSE endpoint to send real-time updates
const ssePoint = (clients) => {
    return (req, res) => {
        // Set headers for SSE
        res.setHeader('Content-Type', 'text/event-stream');
        res.setHeader('Cache-Control', 'no-cache');
        res.setHeader('Connection', 'keep-alive');

        // Generate a unique client ID
        const clientId = uuidv4();

        // Create a new client object and add it to the clients array
        const newClient = { id: clientId, res };
        clients.push(newClient);

        // Handle client disconnects
        req.on('close', () => {
            clients = clients.filter(client => client.id !== clientId);
            console.log(`Client ${clientId} disconnected`);
        });
    };
}

// Xử lý các yêu cầu từ IoT devices
const receiveDataIOT = async (clients,req, res) => {
    if (getAllowDataDisplay()) {
        const { deviceId, soundLevel } = req.body;
        const currentDate = new Date().getTime();

        const params = {
            TableName: 'soundLevel',
            Key: {
                deviceId: deviceId.toString(),
            },
            UpdateExpression: 'SET #listDatas = list_append(if_not_exists(#listDatas, :empty_list), :data)',
            ExpressionAttributeNames: {
                '#listDatas': 'listDatas',
            },
            ExpressionAttributeValues: {
                ':data': [
                    {
                        data: soundLevel.toString(),
                        time: currentDate
                    },
                ],
                ':empty_list': [],
                ':soundLevel': soundLevel.toString(),
            },
            ReturnValues: 'ALL_NEW',
        };

        // Update the item in DynamoDB
        try {
            await dynamodb.update(params).promise();
            console.log('Successfully saved soundLevel to DynamoDB');

            const message = JSON.stringify({
                data: soundLevel.toString(),
                time: currentDate,
            });

            // Gửi dữ liệu qua SSE cho tất cả client đang kết nối
            clients.forEach(client => {
                client.res.write(`data: ${message}\n\n`);
            });



            res.status(200).json({ message: 'Data received and processed' });
        } catch (err) {
            console.error('Unable to update item. Error JSON:', JSON.stringify(err, null, 2));
            res.status(500).json({ error: 'Internal Server Error' });
        }
    } else {
        res.status(403).json({ error: 'Data display not allowed' });
    }
}









// const connectDeviceWaiting = async (req, res) => {
//const {deviceId } = req.body;
//
//     try {
//
//         // Update device status in DynamoDB
//         const params = {
//             TableName: 'Devices', // Replace with your DynamoDB table name
//             Key: { deviceId: deviceId },
//             UpdateExpression: 'set #status = :status',
//             ExpressionAttributeNames: {
//                 '#status': 'status'
//             },
//             ExpressionAttributeValues: {
//                 ':status': true // Update status to true (connected)
//             },
//             ReturnValues: 'UPDATED_NEW'
//         };
//
//         const updatedDevice = await dynamodb.update(params).promise();
//
//         res.status(200).json({ message: 'Device connected successfully', updatedDevice });
//     } catch (error) {
//         console.error('Error connecting device:', error);
//         res.status(500).json({ message: 'Failed to connect device' });
//     }
// }


module.exports = {
    fetchAllDevice,
    connectDeviceWaiting,
    getDataChart,
    ssePoint,
    receiveDataIOT
};
