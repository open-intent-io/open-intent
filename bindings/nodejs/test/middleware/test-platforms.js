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
var should = require('should');
var fs = require('fs');
var path = require('path');
var request = require('supertest');
var crypto = require('crypto');

var Chatbot = require('../../lib/chatbot');
var Platforms = require('../../lib/middleware/platforms');

var SERVICE_PORT = 10300;

describe('Test platforms middleware', function() {
    var file = path.resolve(__dirname, '../res/food_bot/dictionary.json');
    var dictionary = JSON.parse(fs.readFileSync(file, 'utf-8'));

    var file = path.resolve(__dirname, '../res/food_bot/script.txt');
    var oiml = fs.readFileSync(file, 'utf-8');

    var userCommands = require(path.resolve(__dirname, '../res/food_bot/user_commands.js'));

    var botmodel = {
        dictionary: dictionary,
        oiml: oiml,
        user_commands: userCommands
    };

    before(function() {
        process.env.SEND_API_URI = '/';
    });

    after(function() {
        process.env.SEND_API_URI = undefined;
    });

    describe('When attaching the platforms middleware', function() {
        it('should attach the middleware without error', function() {
            var config = {
                general: {
                    port: SERVICE_PORT,
                    baseUrl: '/'
                },
                selection: {
                    messenger: true,
                    kik: false,
                    skype: false,
                    slack: false,
                    telegram: false
                },
                kik: {
                    "username": "abc",
                    "apiKey": "",
                    "baseUrl": ""
                },
                messenger: {
                    "MESSENGER_APP_SECRET": "APP_SECRET",
                    "MESSENGER_PAGE_ACCESS_TOKEN": "PAGE_TOKEN",
                    "MESSENGER_VALIDATION_TOKEN": "VALIDATION_TOKEN"
                },
                skype: {
                    "APP_ID":"",
                    "BOT_ID":"",
                    "APP_SECRET":""
                },
                slack: {
                    "API_TOKEN": ""
                },
                telegram: {
                    "API_TOKEN":""
                }
            };

            var chatbot = new Chatbot();
            var botconfig = {};
            config.middlewares = [ Platforms(config) ]

            return chatbot.start(botmodel, botconfig)
            .then(function() {
                return chatbot.stop();
            });
        });
    });

    describe('When platforms middleware is attached, the chatbot', function() {
        var chatbot;
        var PlatformMiddleware;

        before(function() {
            var config = {
                general: {
                    port: SERVICE_PORT,
                    baseUrl: '/'
                },
                selection: {
                    messenger: true,
                    kik: false,
                    skype: false,
                    slack: false,
                    telegram: false
                },
                messenger: {
                    "MESSENGER_APP_SECRET": "APP_SECRET",
                    "MESSENGER_PAGE_ACCESS_TOKEN": "PAGE_TOKEN",
                    "MESSENGER_VALIDATION_TOKEN": "VALIDATION_TOKE?"
                }
            };


            chatbot = new Chatbot();
            PlatformMiddleware = Platforms(config);

            var botconfig = {};
            botconfig.middlewares = [PlatformMiddleware];

            return chatbot.start(botmodel, botconfig)
        });

        after(function() {
            return chatbot.stop();
        });

        it('should interact with messenger', function(done) {
            var body = {
                object: 'page',
                entry: [
                    {
                        id: 'ENTRY_ID',
                        time: 'CUSTOM_TIME',
                        messaging: [
                            {
                                message: {
                                    text: 'Hello'
                                },
                                sender: {
                                    id: 45465658
                                },
                                recipient: {
                                    id: 45465660
                                },
                                timestamp: 12345660
                            }
                        ]
                    }
                ]
            };

            request(PlatformMiddleware._server)
                .post('/messenger/chat')
                .send(body)
                .set('x-hub-signature', "sha1=46d851011a6b3e870962f1eac707aff70f865729")
                .set('Accept', 'application/json')
                .expect('Content-Type', 'text/plain; charset=utf-8')
                .expect(200)
                .end(function(err, res) {
                    res.text.should.eql('OK');
                    done();
                });
        });

        it('should interact with kik', function() {

        });

        it('should interact with slack', function() {

        });

        it('should interact with skype', function() {

        });

        it('should interact with telegram', function() {

        });
    });
});
