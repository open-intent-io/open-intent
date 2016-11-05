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
var stream = require("mock-utf8-stream");
var Q = require('q');
var fs = require('fs');

var IrcClient = require('../../lib/middleware/irc');
var Chatbot = require('../../lib/chatbot');

var foodBotModel = require('../food-bot-model');

describe('Test the IRC client', function() {
    var botmodel = foodBotModel;
    var talk;

    var createAssertEqFunction = function(input, expected) {
        return function(data) {
            expect(data).to.equal(expected);
            return talk(input);
        }
    };

    var handleScript = function(script, done) {
        var promise = talk(script[0]);
        for(var i=2; i < script.length; i += 2) {
            var input = script[i];
            var expectedOutput = script[i - 1];

            promise = promise
            .then(createAssertEqFunction(input, expectedOutput))
            .fail(function(err) {
                console.error(err);
            });
        }
        
        promise.then(function(data) {
            expect(data).to.equal(script[script.length-1]);
            done();
        })
        .fail(function(err) {
            console.error(err);
        });
    };

    beforeEach(function() {
        var stdinMock = new stream.MockReadableStream();
        var stdoutMock = new stream.MockReadableStream();

        var stdio = {
            stdin: stdinMock,
            stdout: stdoutMock
        };

        talk = function(input) {
            var deferred = Q.defer();

            var onData = function(data) {
                deferred.resolve(data);
                stdoutMock.removeListener('data', onData);
            };

            stdoutMock.on('data', onData);
            stdinMock.write(input);

            return deferred.promise;
        };

        var chatbot = new Chatbot();

        chatbot.set('irc', IrcClient(stdio));

        return chatbot.start(botmodel);
    });
    
    it('should handle a conversation in which the user order a hamburger', function(done) {
        var script = [
            "Hello\n", 
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Hamburger\n", 
            "Got it, you want hamburger, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"
        ];

        handleScript(script, done);
    });

    it('should handle a conversation in which the user order a salad', function(done) {
        var script = [
            "Hello\n", 
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Salad\n", 
            "Got it, you want salad, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"
        ];

        handleScript(script, done);
    });

    it('should handle a conversation in which the user order a pizza', function(done) {
        var script = [
            "Hello\n", 
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Pizza\n", 
            "Congratulations, you get a special offer\n",
            "yes\n",
            "Congratulations on the special offer, it is gonna be 5$.\n"
        ];

        handleScript(script, done);
    });

    it('should handle a conversation in which the chatbot does not understand the item', function(done) {
        var script = [
            "Hello\n", 
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "chicken\n", 
            "I did not understand. Pizza, hamburger or salad?\n"
        ];

        handleScript(script, done);
    });

    it('should handle a conversation in which the user made a mistake during ordering', function(done) {
        var script = [
            "Hello\n", 
            "Would you want to eat a pizza, a hamburger or a salad?\n",
            "Pizza\n", 
            "Congratulations, you get a special offer\n",
            "No\n",
            "Tell me what you want. Pizza, hamburger or salad?\n",
            "Salad\n",
            "Got it, you want salad, right?\n",
            "yes\n",
            "I'm ordering, it is gonna be 5$.\n"

        ];

        handleScript(script, done);
    });
});
