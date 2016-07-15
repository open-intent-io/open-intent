
var expect    = require("chai").expect;
var stream = require("mock-utf8-stream");
var Q = require('q');

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

        chatbot.start(8080, resDirectory, true)
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