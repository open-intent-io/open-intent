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
var request = require('request');
var http = require('http');
var express = require('express');
var bodyParser = require('body-parser');
var compression = require('compression');

var messenger = require('./messenger');


// declare Facebook tokens
var fb_verify_token;
var fb_page_token;
var receiveCallback;

// Express listening
var port = process.env.PORT || 8445;


function Client(sender) {

    this.sessionId = sender;

    this.send = function(message) {
        sendMessage(this.sessionId, message);
    }

    return this;
}


function handleVerify(req, res, next) {
    if (req.query['hub.verify_token'] === fb_verify_token) {
        res.send(req.query['hub.challenge']);
    } else {
        res.send('Error, wrong validation token');    
    }
}

function receiveMessage(req, res, next) {
    messaging_events = req.body.entry[0].messaging;
    for (i = 0; i < messaging_events.length; i++) {
        event = req.body.entry[0].messaging[i];
        sender = event.sender.id;
        if (event.message && event.message.text) {
            text = event.message.text;
            if(receiveCallback) {
                receiveCallback(new Client(sender), text);
            }
        }
    }
    res.sendStatus(200);
}

function sendMessage(sender, text) {
    messageData = {
        text:text
    }

    request({
        url: 'https://graph.facebook.com/v2.6/me/messages',
        qs: { access_token: fb_page_token},
        method: 'POST',
        json: {
            recipient: { id: sender},
            message: messageData,
        }
    }, function(error, response, body) {
        if (error) {
            console.log('Error sending message: ', error);
        } else if (response.body.error) {
            console.log('Error: ', response.body.error);
        }
    });
}


module.exports = function(verifyToken, pageToken, receiveCb) {
    fb_verify_token = verifyToken;
    fb_page_token = pageToken;
    receiveCallback = receiveCb;

    var app = express();
    // Setup express
    app.use(compression());
    app.set('case sensitive routing', true);
    app.use(bodyParser.json());

    app.get('/', function (req, res, next) {
          res.send('Welcome to Facebook Messenger Bot. This is root endpoint');
    });

    app.get('/webhook/', handleVerify);
    app.post('/webhook/', receiveMessage);

    var httpServer = http.createServer(app);
    httpServer.listen(port, function () {
        console.log("Express http server listening on port " + port);
    });


    this.send = sendMessage;
    return this;
}
