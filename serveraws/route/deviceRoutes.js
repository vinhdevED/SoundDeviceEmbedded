const express = require('express');
const router = express.Router();
const {
    fetchAllDevice,
    connectDeviceWaiting,
    getDataChart,
    ssePoint,
    receiveDataIOT
} = require('../controller/deviceController');

module.exports = (clients) => {
    router.get('/devices', fetchAllDevice);
    router.post('/connect-device', connectDeviceWaiting);
    router.get('/soundLevels',getDataChart);
    router.post('/endpoint',  (req,
                                            res) => receiveDataIOT(clients, req, res));

    // SSE endpoint
    router.get('/events', ssePoint(clients));

    return router;
};


// router.get('/devices', fetchAllDevice);
// router.post('/connect-device', connectDeviceWaiting);
// router.get('/soundLevels',getDataChart);
// router.post('/endpoint', receiveDataIOT);
// router.get('/events', ssePoint(clients));
//
// module.exports = router;