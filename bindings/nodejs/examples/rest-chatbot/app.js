/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

The MIT License (MIT)
Copyright (c) 2016 - Clement Michaud, Sergei Kireev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
var OpenIntentChatbot = require('open-intent-chatbot');
var vmUserDefinedActionDriver = require('open-intent-chatbot/user-defined-actions/vm-driver');
var standaloneSessionManager = require('open-intent-chatbot/session-manager/standalone-driver')();

var rest_client = require('./rest-client');
var cors = require('cors');

var express = require('express');
var app = express();
var bodyParser = require('body-parser');
app.use(bodyParser.json());
app.use(cors());

var fs = require('fs');
var file = 'chatbot.json';
var model = fs.readFileSync(file);

var userCommands = require('./userCommands');

var userDefinedActionDriver = vmUserDefinedActionDriver(userCommands);
var chatbot = OpenIntentChatbot.fromJsonModel(model, userDefinedActionDriver, standaloneSessionManager);

app.post('/talk', function(req, res) {
    chatbot.talk(req, res);
});

app.post('/setState', function(req, res) {
    chatbot.setState(req, res);
});

app.get('/getState', function(req, res) {
    chatbot.getState(req, res);
});

app.listen('3001');

// Test with a REST client that sends requests with user input on stdin
var host = 'http://127.0.0.1:3001';
console.log("Chatbot initial state is ", chatbot.getInitialState());
rest_client(host, chatbot.getInitialState());
