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
var seqqueue = require('seq-queue');

var talk = undefined;
var chatbot = undefined;

module.exports = function(resDirectory) {
    var chatbot = require('../chatbot');
    var openintent = require('open-intent');

    var _beforeEach = function(done) {
        var stdinMock = new stream.MockReadableStream();
        var stdoutMock = new stream.MockReadableStream();

        var stdio = {
            stdin: stdinMock,
            stdout: stdoutMock
        };

        var middlewares = [];

        middlewares.push(openintent.middleware.Irc(stdio));

        chatbot(middlewares, function(error) {
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
        });
    };

    return {
        checkScript: handleScript,

        beforeEach: _beforeEach,
        afterEach: _afterEach
    };
};

function buildRequest(request, expectedAnswer, deferred) {
    return function(task) {
        talk(request)
        .then(function(answer) {
            expect(answer).to.equal(expectedAnswer);
            deferred.resolve();
            task.done();
        })
        .fail(function(err) {
            console.error(err);
            deferred.reject();
        });
    };
}

function handleScript(script, done) {
    var queue = seqqueue.createQueue();
    var promises = [];

    for(var i=0; i < script.length; i += 2) {
        var request = script[i] + '\n';
        var answer = script[i+1] + '\n';
        var deferred = Q.defer();
        promises.push(deferred.promise);
        queue.push(buildRequest(request, answer, deferred));
    }

    Q.all(promises).
    then(function() {
        done();
    });
}

function _afterEach(done) {
    if(chatbot) {
        delete chatbot;
    }
    done();
}
