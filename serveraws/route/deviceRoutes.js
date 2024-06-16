const express = require('express');
const router = express.Router();
const { createDevice, fetchDevice ,fetchAllDevice,connectDeviceWaiting} = require('../controller/deviceController');

//router.post('/', createDevice);
//router.get('/:deviceId', fetchDevice);
router.get('/devices', fetchAllDevice);
router.post('/connect-device', connectDeviceWaiting);

module.exports = router;
