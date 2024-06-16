const net = require('net');

const deviceHost = '192.168.1.69'; // Địa chỉ IP của thiết bị IoT
const devicePort = 3000; // Cổng TCP của thiết bị IoT

// Tạo kết nối TCP đến thiết bị IoT
const client = net.createConnection({ host: deviceHost, port: devicePort }, () => {
    console.log('Connected to IoT device');

    // Gửi dữ liệu đến thiết bị IoT
    const message = 'Hello IoT device!';
    client.write(message);

    // Đóng kết nối sau khi gửi dữ liệu
    client.end();
});

// Xử lý khi kết nối bị đóng
client.on('close', () => {
    console.log('Connection to IoT device closed');
});

// Xử lý lỗi khi kết nối không thành công
client.on('error', (err) => {
    console.error('Error connecting to IoT device:', err);
});
