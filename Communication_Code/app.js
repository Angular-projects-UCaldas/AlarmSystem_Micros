'use strict'

const express = require('express');
const app = express();

app.use(express.urlencoded({ extended: false }));

app.use(express.json());

var MessageRoutes = require('./routes/messagesRoutes');
var DriveRoutes = require('./routes/driveRoutes');

app.use('/api', MessageRoutes);
app.use('/api', DriveRoutes);
app.use('/', (req, res) => {
    res.status(200).send('Welcome to the API!');
});

module.exports = app;
