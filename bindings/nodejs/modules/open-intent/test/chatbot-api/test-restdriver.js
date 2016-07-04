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
var assert    = require("chai").assert;
var sinon = require('sinon');

var RestUserCommandsDriver = require('../../chatbot-api/user-defined-actions/rest-driver');

var REST_SERVICE_PORT = process.env.REST_SERVICE_PORT || 23000;
var serviceUrl = 'http://localhost:' + REST_SERVICE_PORT;


describe("Test REST user commands driver", function(endOfTestsCallback) {
    var express = require('express');
    var app = express();


    before(function() {
        app.post('/command1', function(req, res) {
            res.setHeader('Content-Type', 'application/json');
            res.send({'0': 'variable0'});
        });

        app.post('/MyCommand', function(req, res) {
            res.setHeader('Content-Type', 'application/json');
            res.send({'0': 'MyCommandVariable0'});
        });

        app.listen(REST_SERVICE_PORT);
    });

    describe("When user command exists for the actionId, the driver executes it", function() {
        it('should execute the command1', function(done) {
            var userCommandsDriver = new RestUserCommandsDriver(serviceUrl);
            userCommandsDriver.execute('command1', 'SESSION', {})
            .then(function() {
                done();
            });
        });
    });

    describe("When user command does not exist for the actionId, the driver does not execute any command", function() {
        it('should execute the command1', function(done) {
            var userCommandsDriver = new RestUserCommandsDriver(serviceUrl);
            userCommandsDriver.execute('command2', 'SESSION', {})
            .fail( function() {
                done();
            });
        });
    });

    describe("When config contains endpoints the commands must be routed to endpoints", function() {
        var config = {
            'endpoints': {
                'command2': '/MyCommand'
            }
        };

        it('should execute the MyCommand', function(done) {
            var userCommandsDriver = new RestUserCommandsDriver(serviceUrl, config);
            userCommandsDriver.execute('command2', 'SESSION', {})
            .then( function() {
                done();
            });
        });

        it('should fail when attempting to call the original if not mapped', function(done) {
            var userCommandsDriver = new RestUserCommandsDriver(serviceUrl, config);
            userCommandsDriver.execute('command1', 'SESSION', {})
            .fail( function() {
                done();
            });
        });
    });
});
