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
var expect    = require("chai").expect;
var sinon = require('sinon');

var createChatbot = require('../../lib/chatbot-api/chatbot');
var foodBotModel = require('../food-bot-model');
var badFoodBotModel = require('../bad-food-bot-model');

var fs = require('fs');

describe("Test open intent chatbot", function() {
    var botmodel = foodBotModel;

    it('should succeed on getState call', function(done) {
        var SESSION_ID = 'abc';

        createChatbot(botmodel)
        .then(function(chatbot) {
            return chatbot.getState(SESSION_ID);
        })
        .then(function(state) {
            done();
        });
    });

    it('should succeed on setState call', function(done) {
        var SESSION_ID = 'abc';

        createChatbot(botmodel)
        .then(function(chatbot) {
            return chatbot.setState(SESSION_ID, 'state');
        })
        .then(function(state) {
            done();
        })
        .fail(function(err) {
            console.error(err);
        })
    });

    it('should succeed on talk call', function(done) {
        var SESSION_ID = 'abc';

        createChatbot(botmodel)
        .then(function(chatbot) {
            return chatbot.talk(SESSION_ID, 'Hello');
        })
        .then(function(state) {
            done();
        })
        .fail(function(err) {
            console.log(err);
        });
    });

    describe('When right dictionary and js script are provided but bad script', function() {
        var botmodel = badFoodBotModel;

        it('should return the interpreter feedback as error', function(done) {
            createChatbot(botmodel)
            .fail(function (err) {
                expect(err).to.deep.equal({
                    "messages": [
                        {
                            "level": 0,
                            "line": 11,
                            "message": "no entity was found for the line"
                        }
                    ]
                });
                done();
            });
        });
    });
});
