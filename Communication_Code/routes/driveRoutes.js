'user strict'

var express = require('express');
var DriveController = require('../controllers/googleDriveController');
var api = express.Router();

api.post('/saveImage', DriveController.uploadImage);

module.exports = api;