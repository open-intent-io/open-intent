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

var OpenIntentChatbot = require('../../chatbot-api/chatbot');

var fs = require('fs');

var file = 'test/chatbot-api/res/chatbot.json';
var jsonModel = fs.readFileSync(file);

describe("Test open intent chatbot", function() {

    describe("Almost all methods should fail when there is no model", function () {
        
        it('should fail when getState is called', function(done) {
            var chatbot = OpenIntentChatbot();
            var SESSION_ID = 'abc';

            chatbot.getState(SESSION_ID)
            .fail(function() {
                done();
            });
        });

        it('should fail when setState is called', function(done) {
            var chatbot = OpenIntentChatbot();
            var SESSION_ID = 'abc';

            chatbot.setState(SESSION_ID)
            .fail(function() {
                done();
            });
        });

        it('should fail when talk is called', function(done) {
            var chatbot = OpenIntentChatbot();
            var SESSION_ID = 'abc';

            chatbot.talk(SESSION_ID, 'Hello')
            .fail(function() {
                done();
            });
        });

        it('should fail when getModel is called', function(done) {
            var chatbot = OpenIntentChatbot();

            chatbot.getModel()
            .fail(function() {
                done();
            });
        });
    });

    describe('When a good model is provided', function() {
        it('should correctly update the model', function(done) {
            var chatbot = OpenIntentChatbot();

            var botmodel = {
                'commands': {
                    'type': 'js',
                    'script': 'module.exports = {}'
                },
                'model': {
                    'json': jsonModel
                }
            };
            
            chatbot.setModel(botmodel)
            .then(function() {
                done();
            });
        })
    });

    describe('When a model is loaded', function() {
        it('should return the initial state when getstate is called on any session', function(done) {
            var chatbot = new OpenIntentChatbot();

            var botmodel = {
                'commands': {
                    'type': 'js',
                    'script': 'module.exports = {}'
                },
                'model': {
                    'json': jsonModel
                }
            };

            chatbot.setModel(botmodel)
            .then(function() {
                return chatbot.getState('ABC');
            })
            .then(function(state) {
                expect(state).to.equal('init');
                done();
            });
        });
    });

    describe('When updating the model several times', function() {
        it('should update correctly the next models', function(done) {
            var chatbot = OpenIntentChatbot();

            var botmodel1 = {
                'commands': {
                    'type': 'js',
                    'script': 'module.exports = {}'
                },
                'model': {
                    'json': jsonModel
                }
            };
            
            var botmodel2 = {
                'commands': {
                    'type': 'js',
                    'script': 'module.exports = { "command1": function()) {} }'
                },
                'model': {
                    'json': jsonModel
                }
            };

            chatbot.setModel(botmodel1)
            .then(function() {
                return chatbot.setModel(botmodel2);
            })
            .then(function() {
                done();
            })
        })
    });

    describe('After updating the model all methods should be callable', function() {
        var botmodel = {
            'commands': {
                'type': 'js',
                'script': 'module.exports = {}'
            },
            'model': {
                'json': jsonModel
            }
        };

        it('should succeed on getState call', function(done) {
            var chatbot = OpenIntentChatbot();
            var SESSION_ID = 'abc';

            chatbot.setModel(botmodel)
            .then(function() {
                return chatbot.getState(SESSION_ID);
            })
            .then(function(state) {
                done();
            });
        });

        it('should succeed on setState call', function(done) {
            var chatbot = OpenIntentChatbot();
            var SESSION_ID = 'abc';

            chatbot.setModel(botmodel)
            .then(function() {
                return chatbot.setState(SESSION_ID, 'state');
            })
            .then(function() {
                done();
            });
        });

        it('should succeed on getModel call', function(done) {
            var chatbot = OpenIntentChatbot();

            chatbot.setModel(botmodel)
            .then(function() {
                return chatbot.getModel();
            })
            .then(function(model) {
                done();
            });
        });

        it('should succeed on talk call', function(done) {
            var chatbot = OpenIntentChatbot();
            var SESSION_ID = 'abc';

            chatbot.setModel(botmodel)
            .then(function() {
                return chatbot.talk(SESSION_ID, 'Bob');
            })
            .then(function(replies) {
                done();
            }).fail(function(err) {
                console.log(err);
            });
        });
    });

    describe('When right script, dictionary and js script are provided', function() {
        var file = 'test/chatbot-api/res/food_bot/dictionary.json';
        var dictionary = fs.readFileSync(file, 'utf-8');

        var file = 'test/chatbot-api/res/food_bot/script.txt';
        var oimlScript = fs.readFileSync(file, 'utf-8');

        var file = 'test/chatbot-api/res/food_bot/user_commands.js';
        var user_commands = fs.readFileSync(file, 'utf-8');

        var botmodel = {
            'commands': {
                'type': 'js',
                'script': user_commands
            },
            'model': {
                'dictionary': dictionary,
                'script': oimlScript
            }
        };

        it('should create the chatbot without any error', function(done) {
            var chatbot = OpenIntentChatbot();

            chatbot.setModel(botmodel)
            .then(function() {
                done();
            })
            .fail(function(err) {
                console.log(err);
            });
        });
    });

    describe('When right dictionary and js script are provided but bad script', function() {
        var file = 'test/chatbot-api/res/food_bot/dictionary.json';
        var dictionary = fs.readFileSync(file, 'utf-8');

        var file = 'test/chatbot-api/res/food_bot/bad_script.txt';
        var oimlScript = fs.readFileSync(file, 'utf-8');

        var file = 'test/chatbot-api/res/food_bot/user_commands.js';
        var user_commands = fs.readFileSync(file, 'utf-8');

        var botmodel = {
            'commands': {
                'type': 'js',
                'script': user_commands
            },
            'model': {
                'dictionary': dictionary,
                'script': oimlScript
            }
        };

        it('should return the interpreter feedback as error', function(done) {
            var chatbot = OpenIntentChatbot();

            chatbot.setModel(botmodel)
            .fail(function (err) {
                expect(err).to.deep.equal({
                    "messages": [
                        {
                            "level": 0,
                            "line": 11,
                            "message": "no entity was found for the line"
                        },
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

    describe('When redis session manager is selected', function() {

        it('should contain a RedisSessionManager interface', function() {
            var config = {
                'session': {
                    'type': 'redis',
                    'config': {
                        'host': 'http://localhost',
                        'port': 6379
                    }
                }
            };

            var chatbot = OpenIntentChatbot(config);
            expect(chatbot._sessionManager);
        });

    });
});
