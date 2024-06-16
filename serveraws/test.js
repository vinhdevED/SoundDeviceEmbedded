// const net = require('net');
//
// const deviceHost = '192.168.1.69'; // Địa chỉ IP của thiết bị IoT
// const devicePort = 3000; // Cổng TCP của thiết bị IoT
//
// // Tạo kết nối TCP đến thiết bị IoT
// const client = net.createConnection({ host: deviceHost, port: devicePort }, () => {
//     console.log('Connected to IoT device');
//
//     // Gửi dữ liệu đến thiết bị IoT
//     const message = 'Hello IoT device!';
//     client.write(message);
//
//     // Đóng kết nối sau khi gửi dữ liệu
//     client.end();
// });
//
// // Xử lý khi kết nối bị đóng
// client.on('close', () => {
//     console.log('Connection to IoT device closed');
// });
//
// // Xử lý lỗi khi kết nối không thành công
// client.on('error', (err) => {
//     console.error('Error connecting to IoT device:', err);
// });

// import fetch from 'node-fetch';
//
//
// async function sendDataToDevice(deviceIP, port, data) {
//     const url = `http://${deviceIP}:${port}/endpoint`; // Thay đổi '/endpoint' thành endpoint thực tế của thiết bị IoT
//
//     try {
//         const response = await fetch(url, {
//             method: 'POST',
//             headers: {
//                 'Content-Type': 'application/json',
//             },
//             body: JSON.stringify(data),
//         });
//
//         if (response.ok) {
//             console.log('Data sent successfully');
//         } else {
//             console.error('Failed to send data:', response.status);
//         }
//     } catch (error) {
//         console.error('Error sending data:', error);
//     }
// }
//
// // Gọi hàm để gửi dữ liệu đến thiết bị IoT
// sendDataToDevice('192.168.1.69', 80, { message: 'Hello IoT Device' });

import http from 'http';

// Define the options for the HTTP request
const options = {
    hostname: '192.168.1.69', // Replace with your IoT device's IP address
    port: 80, // Replace with your IoT device's port number
    path: '/send-message', // Replace with the endpoint on your IoT device
    method: 'POST', // HTTP method
    headers: {
        'Content-Type': 'application/json',
    }
};

// Data to send to IoT device
const data = JSON.stringify({
    message: 'Hello IoT Device!'
});

// Create a HTTP request object
const req = http.request(options, (res) => {
    console.log(`Status code: ${res.statusCode}`);

    // Collect response data
    let responseData = '';
    res.on('data', (chunk) => {
        responseData += chunk;
    });

    // Process response when complete
    res.on('end', () => {
        console.log('Response from IoT device:');
        console.log(responseData);
    });
});

// Handle errors
req.on('error', (error) => {
    console.error('Error connecting to IoT device:', error);
});

// Write data to request body
req.write(data);
req.end(); // Close the request

