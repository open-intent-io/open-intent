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
var should    = require("should");
var sinon = require('sinon');
var fs = require('fs');

var VMUserDefinedActionDriver = require('../../lib/chatbot-api/user-defined-actions/vm-driver');

describe("Test VM user commands driver", function() {

    describe("When user command exists for the actionId, the driver executes it", function(done) {
        it('should execute the command1', function() {
            var userCommands = {
                'command1': function(intentVariables, sessionId, next) {
                    next();
                },
                'command2': function(intentVariables, sessionId, next) {
                    next();
                }
            }

            var userCommandsDriver = new VMUserDefinedActionDriver(JSON.stringify(userCommands));
            userCommandsDriver.execute('command1', 'SESSION', {})
            .then(function() {
                done();
            });
        });
    });

    describe("When user command does not exist for the actionId, the driver does not execute any command", function() {
        it('should call next callback even if there is no user command', function(done) {
            var userCommands = {
                'command1': function(intentVariables, sessionId, next) {
                    next();
                },
                'command2': function(intentVariables, sessionId, next) {
                    next();
                }
            }

            var userCommandsDriver = new VMUserDefinedActionDriver(JSON.stringify(userCommands));
            userCommandsDriver.execute('unknown_command', 'SESSION', {})
            .then(function() {
                done();
            });
        });
    });
    
    describe("The VM keeps a state between calls", function() {
        describe('Create VM from module', function() {
            it('should keep a variable state', function(done) {
                var userCommands = require('./vm-commands');

                var userCommandsDriver = new VMUserDefinedActionDriver(userCommands);
                userCommandsDriver.execute('command1', 'SESSION', {})
                    .then(function(userVariables) {
                        should.equal(userVariables.state, 5);
                        return userCommandsDriver.execute('command2', 'SESSION', {});
                    })
                    .then(function(userVariables) {
                        should.equal(userVariables.state, 7);
                        done();
                    });
            });
        });

        describe('Create VM from string', function() {
            it('should keep a variable state', function(done) {
                var path = require('path');
                var userCommands = fs.readFileSync(path.resolve(__dirname, 'vm-commands.js'), 'utf-8');

                var userCommandsDriver = new VMUserDefinedActionDriver(userCommands);
                userCommandsDriver.execute('command1', 'SESSION', {})
                    .then(function(userVariables) {
                        should.equal(userVariables.state, 5);
                        return userCommandsDriver.execute('command2', 'SESSION', {});
                    })
                    .then(function(userVariables) {
                        should.equal(userVariables.state, 7);
                        done();
                    });
            });
        });
    });
});
