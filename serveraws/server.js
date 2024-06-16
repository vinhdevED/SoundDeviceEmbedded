require('dotenv').config();
const express = require('express');
const app = express();
const path = require('path');
const deviceRoutes = require('./route/deviceRoutes');
const { connectToDynamoDB } = require('./config/aws-config');

const PORT = process.env.PORT || 3000;

// Middleware
app.use(express.static(path.join(__dirname, 'template')));
app.use(express.json())

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'template', 'web.html'));
});

// Mount deviceRoutes without any prefix
app.use('/', deviceRoutes);

// Khởi động server
app.listen(PORT, async () => {
    await connectToDynamoDB();
    console.log(`Server is running on port ${PORT}`);
});
