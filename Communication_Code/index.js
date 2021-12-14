'use strict'

const port = process.env.PORT || 80;

const app = require('./app');

app.listen(port, () => {
    console.log(`Listening on port: ${port}`)
});