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
var sinon = require('sinon');
var assert = require('assert');

var Chatbot = require('../lib/chatbot');
var foodBotModel = require('./food-bot-model');

describe('Test Chatbot with middlewares', function() {

    it('should call setup of the user commands when the chatbot starts', function(done) {
        var chatbot = new Chatbot();
        var config = {};
        var setup_spy = sinon.spy();

        foodBotModel.user_commands = function(handler) {
            handler.on('_setup', function(next) {
                setup_spy();
                next();
            });
        };

        chatbot.start(foodBotModel, config)
        .then(function() {
            assert(setup_spy.calledOnce);
            done();
        })
        .fail(function(error) {
            console.error('Error', error);
        });
    });

    it('should call cleanup of the user commands when the chatbot stops', function(done) {
        var chatbot = new Chatbot();
        var config = {};
        var spy = sinon.spy();

        foodBotModel.user_commands = function(handler) {
            handler.on('_cleanup', function(next) {
                spy();
                next();
            });
        };

        chatbot.start(foodBotModel, config)
        .then(function() {
            return chatbot.stop()
        })
        .then(function() {
            assert(spy.calledOnce);
            done();
        })
        .fail(function(error) {
            console.error('Error', error);
        });
    });
});
