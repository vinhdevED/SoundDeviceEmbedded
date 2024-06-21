require('dotenv').config();
const express = require('express');
const app = express();
const path = require('path');
const deviceRoutes = require('./route/deviceRoutes');
const { connectToDynamoDB } = require('./config/aws-config');

const EXPRESSPORT = process.env.EXPRESSPORT || 3000;
const HTTPPORT = process.env.HTTPPORT || 4000;

// Store connections for SSE
let clients = [];

// Middleware
app.use(express.static(path.join(__dirname, 'template')));
app.use(express.urlencoded({ extended: true }));
app.use(express.json())

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'template', 'web.html'));
});

// Mount deviceRoutes without any prefix
app.use('/',  deviceRoutes(clients));


// Khởi động server
app.listen(EXPRESSPORT, async () => {
    await connectToDynamoDB();
    console.log(`Server is running on port ${EXPRESSPORT}`);
});
