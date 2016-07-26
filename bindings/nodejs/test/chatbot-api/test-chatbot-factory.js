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
var Q = require('q');
var path = require('path');

var StandaloneSessionManager = require('../../lib/chatbot-api/session-manager/standalone-driver');
var VMUserDefinedActionDriver = require('../../lib/chatbot-api/user-defined-actions/vm-driver');

var OpenIntentChatbot = require('../../lib/chatbot-api/chatbot-factory');

var fs = require('fs');
var file = path.resolve(__dirname, 'res/chatbot.json');
var model = fs.readFileSync(file, "utf-8");

var file = path.resolve(__dirname, 'res/userCommands.js');
var userCommands = fs.readFileSync(file, "utf-8");

var file = path.resolve(__dirname, 'res/interpreter_model.txt');
var interpreterModel = fs.readFileSync(file, "utf-8");

describe("Test Open Intent chatbot factory", function() {

    describe("Get the chatbot state initially", function() {
        it("should return the initial state of the chatbot", function(done) {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);
            var sessionId = 'SESSION_ID';
            var getStateCallback = sinon.spy();

            assert.isNotNull(chatbot);

            chatbot.getState(sessionId).then(function(state) {
                assert.equal(chatbot.getInitialState(), 'init');
                assert.equal(state, 'init');
                getStateCallback.apply();
                done();
            });
        });
    });

    describe("Set the state to initial state initially", function() {
        it('should return the state "abc"', function(done) {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);
            var sessionId = 'SESSION_ID';
            var STATE = 'abc';
            var getStateCallback = sinon.spy();

            chatbot.setState(sessionId, STATE)
            .then(function() {
                return chatbot.getState(sessionId);
            })
            .then(function(state) {
                expect(state).to.equal(STATE);
                done();
            });
        });
    });

    describe("Somebody calls the waiter", function() {
        it("should ask what to order", function (done) {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);
            var sessionId = 'SESSION_ID';

            chatbot.setState(sessionId, 'init').then(function() {
                return chatbot.talk(sessionId, 'Bob');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(["Que puis-je vous offrir ?"]);
                done();
            });
        });
    });

    describe("Order 2 beverages in one shot", function() {
        it('should follow the conversation', function(done) {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);
            var sessionId = 'SESSION_ID';

            chatbot.setState(sessionId, 'init')
            .then(function() {
                return chatbot.talk(sessionId, 'Bob');
            })
            .then(function(replies) {
                //expect(replies).to.deep.equal(["Que puis-je vous offrir ?"]);
                return chatbot.talk(sessionId, 'Un Coca-Cola');
            })
            .then(function(replies) {
                //expect(replies).to.deep.equal(["Vous-voulez quelque chose d'autre ?"]);
                return chatbot.talk(sessionId, 'rien');
            })
            .then(function(replies) {
                //expect(replies).to.deep.equal(["Au revoir et à bientôt."]);
                done();
            });
        });
    })

    describe("Test interactions with several sessions", function() {

        it("should handle 2 conversations", function(done) {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);
            var sessionId1 = 'SESSION_ID1';
            var sessionId2 = 'SESSION_ID2';

            chatbot.setState(sessionId1, 'init').then(function() {
                return chatbot.setState(sessionId2, 'init');
            })
            .then(function() {
                return chatbot.talk(sessionId1, 'Bob');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(["Que puis-je vous offrir ?"]);
                return chatbot.talk(sessionId2, 'Bob');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(["Que puis-je vous offrir ?"]);
                return chatbot.talk(sessionId1, 'Un coca et une kro');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(["Vous-voulez quelque chose d'autre ?"]);
                return chatbot.talk(sessionId2, 'Blablabla');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(["Je ne comprends pas."]);
                return chatbot.talk(sessionId1, 'rien');
            })
            .then(function(replies) {
                expect(replies).to.deep.equal(["Au revoir et à bientôt."]);
                done();
            });
        });
    });

    describe("The chatbot returns the right initial state", function() {

        it('should return the initial state', function() {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);

            assert.equal(chatbot.getInitialState(), "init");
        })
    });

    describe("The chatbot returns the right terminal states", function() {
        it('should return the terminal states', function() {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);

            assert.sameMembers(chatbot.getTerminalStates(), ["bye", "grab_it"]);
        })
    });

    describe("When a given session talking to the chatbot is not stored, the chatbot starts from initial state", function() {
        it('should handle the message from initial state', function(done) {
            var sessionManager = new StandaloneSessionManager();
            var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);

            chatbot.talk('ABC', "Bob")
            .then(function(replies) {
                done();
            });
        })
    });

    describe("The user commands are correctly called", function() {
        it('should call the user defined command', function(done) {
            var sessionManager = new StandaloneSessionManager();
            var executeMock = sinon.spy();
            var UserCommandsDriverMock = function() {
                this.execute = function(actionId, sessionId, intentVariables) {
                    var defer = Q.defer();
                    var args = [actionId, sessionId, intentVariables];
                    executeMock.apply(this, args);
                    defer.resolve();
                    return defer.promise;
                }
            };

            var userDefinedActionDriver = new UserCommandsDriverMock();
            var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager, userDefinedActionDriver);
            var sessionId = 'SESSION_ID';

            chatbot.setState(sessionId, 'wait_order').then(function() {
                return chatbot.talk(sessionId, 'Je veux un Coca, stp');
            })
            .then(function(replies) {
                assert.isTrue(executeMock.calledWith('append_order1', sessionId, sinon.match.any));
                done();
            });
        })
    })

    describe("Test error handling", function() {
        describe("No user defined actions driver provided", function() {
            it('should accept it and create the chatbot, there should not be any reply template', function(done) {
                var sessionManager = new StandaloneSessionManager();
                var chatbot = OpenIntentChatbot.fromJsonModel(model, sessionManager);
                var sessionId = 'SESSION_ID';

                assert.isNotNull(chatbot);

                chatbot.setState(sessionId, 'wait_order')
                .then(function() {
                    return chatbot.talk(sessionId, 'Je veux un Coca, stp');
                })
                .then(function(replies) {
                    expect(replies).to.deep.equal(["Vous-voulez quelque chose d'autre ?"])
                    done();
                })
            })
        });

        describe("No session manager driver provided", function() {

            it('should throw an exception and not create the chatbot', function() {
                var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
                var chatbot = undefined;

                var fn = function() {
                    chatbot = OpenIntentChatbot.fromJsonModel(model, undefined, userDefinedActionDriver);
                }

                expect(fn).to.throw(Error, 'No session manager driver');
                assert.isUndefined(chatbot);
            });
        });
    });

    describe("When a OIML script is provided", function() {
        describe("When the OIML is right", function() {
            it('should create the corresponding chatbot and interact with it', function(done) {
                var sessionManager = new StandaloneSessionManager();
                var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);
                var chatbot = OpenIntentChatbot.fromOIML(model, interpreterModel, sessionManager,
                    userDefinedActionDriver);
                var SESSION_ID = 'abc';

                chatbot.talk(SESSION_ID, 'Bob')
                    .then(function(replies) {
                        expect(replies).to.deep.equal(['Que puis-je vous offrir ?']);
                        return chatbot.talk(SESSION_ID, 'Je veux un coca et une Kro');
                    })
                    .then(function(replies) {
                        expect(replies).to.deep.equal(['Vous-voulez quelque chose d\'autre ?']);
                        done();
                    })
                    .fail(function(err) {
                        console.log(err);
                    });
            });
        });

        describe("When the OIML is bad", function() {
            it('should throw with an interpreter feedback containing the issues', function() {
                var sessionManager = new StandaloneSessionManager();
                var userDefinedActionDriver = new VMUserDefinedActionDriver(userCommands);

                var file = path.resolve(__dirname, 'res/bad_interpreter_model.txt');
                var badInterpreterModel = fs.readFileSync(file, "utf-8");
                var chatbot = undefined

                var fn = function() {
                    chatbot = OpenIntentChatbot.fromOIML(model, badInterpreterModel,
                        sessionManager, userDefinedActionDriver);
                }

                var interpreterMessages = {
                    'messages': [
                        {
                            level: 0,
                            line: 7,
                            message: "no entity was found for the line"
                        },
                        {
                            level: 0,
                            line: 17,
                            message: "no action was found for the edge"
                        },
                        {
                            level: 0,
                            line: 7,
                            message: "no entity was found for the line"
                        },
                        {
                            level: 0,
                            line: 17,
                            message: "no action was found for the edge"
                        }
                    ]
                };

                expect(fn).to.throw(interpreterMessages);
                assert.isUndefined(chatbot);
            });
        });
    });
});
