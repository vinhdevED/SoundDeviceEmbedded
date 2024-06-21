const express = require('express');
const router = express.Router();
const {
    fetchAllDevice,
    connectDeviceWaiting,
    getDataChart,
    ssePoint,
    receiveDataIOT,
    fetchLastSoundSensor
} = require('../controller/deviceController');

module.exports = (clients) => {
    router.get('/devices', fetchAllDevice);
    router.post('/connect-device', connectDeviceWaiting);
    router.get('/soundLevels',getDataChart);
    router.post('/endpoint',  (req,
                                            res) => receiveDataIOT(clients, req, res));

    // SSE endpoint
    router.get('/events', ssePoint(clients));
    router.get('/lastSoundLevels', fetchLastSoundSensor);


    return router;
};
