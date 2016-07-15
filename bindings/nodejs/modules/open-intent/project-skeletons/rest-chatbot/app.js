
var RestChatbot = require('./rest-chatbot');
var path = require('path');

var modelDirectory = path.join(__dirname, 'res');
var SERVICE_PORT = 8080;
var withIRCClient = true;

var stdio = {
    stdin: process.stdin,
    stdout: process.stdout
}

RestChatbot(stdio).start(SERVICE_PORT, modelDirectory, withIRCClient);


