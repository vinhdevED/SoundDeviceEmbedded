const http = require('http');

const server = http.createServer((req, res) => {
    if (req.method === 'POST' && req.url === '/endpoint') {
        let body = '';
        req.on('data', (chunk) => {
            body += chunk.toString(); // Nhận dữ liệu từ thiết bị IoT
        });
        req.on('end', () => {
            console.log('Received data:', body);
            // Xử lý dữ liệu nhận được từ thiết bị IoT
            res.writeHead(200, {'Content-Type': 'text/plain'});
            res.end('Data received'); // Phản hồi thiết bị IoT
        });
    } else {
        res.writeHead(404, {'Content-Type': 'text/plain'});
        res.end('Endpoint not found');
    }
});

const PORT = 80;
server.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});
