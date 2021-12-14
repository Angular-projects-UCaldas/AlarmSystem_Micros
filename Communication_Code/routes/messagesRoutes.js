'use strict'

var express = require('express');
var MessagesController = require('../controllers/messagesController');
var api = express.Router();
var cors = require('cors');

api.use(cors());

api.post('/sendmessage', MessagesController.sendMessage);
api.post('/getmessage', MessagesController.receiveMessage);
api.post('/code', MessagesController.getCode);
api.get('/getstatus', MessagesController.getStatus);
module.exports = api;