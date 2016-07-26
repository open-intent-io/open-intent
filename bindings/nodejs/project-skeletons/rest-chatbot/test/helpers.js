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

var CHATBOT_PORT = 10005;

var talk = undefined;
var chatbot = undefined;

module.exports = function(resDirectory) {
    var RestChatbot = require('../rest-chatbot');

    var _beforeEach = function(done) {
        var stdinMock = new stream.MockReadableStream();
        var stdoutMock = new stream.MockReadableStream();

        var stdio = {
            stdin: stdinMock,
            stdout: stdoutMock,
        };

        chatbot = RestChatbot(stdio);

        chatbot.start(CHATBOT_PORT, resDirectory, true)
        .then(function() {
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
            done();
        })
        .fail(function(err) {
            console.error('Error: ' + err);
        });
    };

    return {
        checkScript: handleScript,

        beforeEach: _beforeEach,
        afterEach: _afterEach
    };
}

function createAssertEqFunction(input, expected) {
    return function(data) {
        expect(data).to.equal(expected);
        return talk(input);
    }
}

function handleScript(script, done) {
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
}

function _afterEach(done) {
    if(chatbot) {
        chatbot.stop();
        delete chatbot;
    }
    done();
}
