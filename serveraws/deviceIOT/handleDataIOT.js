
const AWS = require('aws-sdk');
const { getAllowDataDisplay } = require('../deviceIOT/allowDataDisplay')
const dynamodb = new AWS.DynamoDB.DocumentClient();

module.exports = (clients) => {
    return (server) => {
        server.on('request', (req, res) => {
            if (req.method === 'POST' && req.url === '/endpoint') {
                let body = '';
                req.on('data', (chunk) => {
                    body += chunk.toString(); // Nhận dữ liệu từ thiết bị IoT
                });
                req.on('end', async () => {
                    if (getAllowDataDisplay()) {
                        const data = JSON.parse(body);
                        const { deviceId, soundLevel } = data;
                        const currentDate = new Date().getTime();

                        const params = {
                            TableName: 'soundLevel', // Tên bảng DynamoDB của bạn
                            Key: {
                                deviceId: deviceId.toString(), // ID thiết bị
                            },
                            UpdateExpression: 'SET #listDatas = list_append(if_not_exists(#listDatas, :empty_list), :data)',
                            ExpressionAttributeNames: {
                                '#listDatas': 'listDatas',
                            },
                            ExpressionAttributeValues: {
                                ':data': [
                                    {
                                        data: soundLevel.toString(), // Dữ liệu âm thanh (dưới dạng chuỗi)
                                        time: currentDate // Thời gian hiện tại
                                    },
                                ],
                                ':empty_list': [], // Danh sách rỗng ban đầu nếu listDatas chưa tồn tại
                            },
                            ReturnValues: 'ALL_NEW', // Trả về tất cả các thuộc tính của mục sau khi cập nhật
                        };

                        // Update the item in DynamoDB
                        dynamodb.update(params, (err, data) => {
                            if (err) {
                                console.error('Unable to update item. Error JSON:', JSON.stringify(err, null, 2));
                            } else {
                                console.log('Successfully saved soundLevel to DynamoDB');
                                const message = JSON.stringify({
                                    data: soundLevel.toString(),
                                    time: currentDate,
                                });

                                // Gửi dữ liệu qua SSE cho tất cả client đang kết nối
                                clients.forEach(client => {
                                    client.res.write(`data: ${message}\n\n`);
                                });
                            }
                        });
                    }
                });
            } else {
                res.writeHead(404, { 'Content-Type': 'text/plain' });
                res.end('Endpoint not found');
            }
        });
    };
};

// const server = http.createServer((req, res) => {
//     if (req.method === 'POST' && req.url === '/endpoint') {
//         let body = '';
//         req.on('data', (chunk) => {
//             body += chunk.toString(); // Nhận dữ liệu từ thiết bị IoT
//         });
//         req.on('end', async () => {
//
//                 if (getAllowDataDisplay()) {
//                     const data = JSON.parse(body);
//                     const {deviceId, soundLevel} = data;
//                     const currentDate = new Date().getTime();
//
//                     // console.log('Received data - Device ID:', deviceId);
//                     // console.log('Received data - Sound Level:', soundLevel);
//
//
//                     const params = {
//                         TableName: 'soundLevel', // Tên bảng DynamoDB của bạn
//                         Key: {
//                             deviceId: deviceId.toString(), // ID thiết bị
//                         },
//                         UpdateExpression: 'SET #listDatas = list_append(if_not_exists(#listDatas, :empty_list), :data)',
//                         ExpressionAttributeNames: {
//                             '#listDatas': 'listDatas',
//                         },
//                         ExpressionAttributeValues: {
//                             ':data': [
//                                 {
//                                     data: soundLevel.toString(), // Dữ liệu âm thanh (dưới dạng chuỗi)
//                                     time: currentDate // Thời gian hiện tại
//                                 },
//                             ],
//                             ':empty_list': [], // Danh sách rỗng ban đầu nếu listDatas chưa tồn tại
//                         },
//                         ReturnValues: 'ALL_NEW', // Trả về tất cả các thuộc tính của mục sau khi cập nhật
//                     };
//
//                     // Update the item in DynamoDB
//                     dynamodb.update(params, (err, data) => {
//                         if (err) {
//                             console.error('Unable to update item. Error JSON:', JSON.stringify(err, null, 2));
//                         } else {
//                             console.log('Successfully saved soundLevel to DynamoDB');
//                             const message = JSON.stringify({
//                                 data: soundLevel.toString(),
//                                 time: currentDate,
//
//                             });
//                         }
//                     });
//                 }
//
//         });
//     } else {
//         res.writeHead(404, {'Content-Type': 'text/plain'});
//         res.end('Endpoint not found');
//     }
// });
//
//
//
//
// module.exports = server;
