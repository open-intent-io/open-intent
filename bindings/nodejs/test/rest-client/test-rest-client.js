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
var fs = require('fs');
var foodBotModel = require('../food-bot-model');
var Rest = require('../../lib/middleware/rest');
var Chatbot = require('../../lib/chatbot');
var ChatbotClient = require('../../lib/chatbot-client');

var SERVICE_HOST = 'http://127.0.0.1';
var SERVICE_PORT = 10105;

describe('Testing the chatbot client of REST API', function() {
    var botmodel = foodBotModel;
    var chatbot = undefined;
    var middleware = undefined;

    describe('Interact with the chatbot', function() {
        before(function(done) {
            middleware = Rest(SERVICE_PORT);
            chatbot = new Chatbot();
            chatbot.set('rest', Rest(SERVICE_PORT));
            chatbot.start(botmodel)
            .then(function() {
                done();
            });
        });

        after(function() {
            return chatbot.stop();
        });

        it('should handle a static scenario correctly', function(done) {
            client = new ChatbotClient(SERVICE_HOST + ':' + SERVICE_PORT);
            var SESSION_ID = 'ABC';

            client.talk(SESSION_ID, 'Hello')
            .then(function(replies) {
                expect(replies).to.deep.equal(['Would you want to eat a pizza, a hamburger or a salad?']);
                return client.talk(SESSION_ID, 'I want a burger');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(['Got it, you want hamburger, right?']);
                return client.talk(SESSION_ID, 'yes');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(['I\'m ordering, it is gonna be 5$.']);
                done();
            })
            .fail(function(error) {
                console.error(error);
            })
        });
    });

    describe('Get state on initialized chatbot', function() {
        before(function(done) {
            var chatbot = new Chatbot();
            chatbot.set('rest', Rest(SERVICE_PORT));

            chatbot.start(botmodel)
            .then(function() {
                done();
            });
        });

        after(function() {
            return chatbot.stop();
        });

        it('should set and return the state correctly when asked', function() {
            client = ChatbotClient(SERVICE_HOST + ':' + SERVICE_PORT);
            var SESSION1 = 'sess1';
            var SESSION2 = 'sess2'

            client.setState(SESSION1, 'state1')
            .then(function() {
                return client.getState(SESSION1);
            })
            .then(function(state) {
                expect(state).to.equal('state1');
                return client.setState(SESSION2, 'state2');
            })
            .then(function() {
                return client.setState(SESSION1, 'state11');
            })
            .then(function() {
                return client.getState(SESSION2);
            })
            .then(function(state) {
                expect(state).to.equal('state2');
                return client.getState(SESSION1);
            })
            .then(function(state) {
                expect(state).to.equal('state11');
                done();
            })
        });
    });
});
