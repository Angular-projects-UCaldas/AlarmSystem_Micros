"use strict";

const env = require("../environments/environment");
const accountSid = env.environment.TWILIO_ACCOUNT_SID;
const authToken = env.environment.TWILIO_AUTH_TOKEN;
const client = require("twilio")(accountSid, authToken);
var status = false;

const sendMessage = (req, res) => {
  console.log(req.body);
  let message = req.body.message;
  let phoneNumber = req.body.phoneNumber;
  status = true;
  client.messages
    .create({
      from: "whatsapp:+14155238886",
      body: message,
      to: "whatsapp:+57" + phoneNumber,
    })
    .then((m) => console.log(m.sid));
  res.status(200).send({ message: "message sent" });
};

const receiveMessage = (req, res) => {
  if (req.body.Body == "12345") {
    status = false;
    client.messages
      .create({
        from: "whatsapp:+14155238886",
        body: "The alarm has been turned off",
        to: "whatsapp:+573136367416",
      })
      .then((m) => console.log(m.sid));
    res.status(200).send({ message: "message sent" });
  } else {
    client.messages
      .create({
        from: "whatsapp:+14155238886",
        body: "Incorrect Password, please try again",
        to: "whatsapp:+573136367416",
      })
      .then((m) => console.log(m.sid));
    res.status(200).send({ message: "message sent" });
  }
  res.status(200).send({ message: "message received" });
};

const getStatus = (req, res) => {
  if (status) {
    res.status(200).send({ message: "Turn off" });
  } else {
    res.status(403).send({ message: "Forbidden" });
  }
};

const getCode = (req, res) => {
  console.log(req.body.MessageStatus);
  res.status(200).send({ message: "Code ok" });
};

const sendMediaMessage = (url, message, phoneNumber) => {
  client.messages
    .create({
      from: "whatsapp:+14155238886",
      mediaUrl: url,
      body: message,
      to: "whatsapp:+57" + phoneNumber,
    })
    .then((m) => console.log(m.sid));
};

module.exports = {
  sendMessage,
  receiveMessage,
  getCode,
  sendMediaMessage,
  getStatus,
};
