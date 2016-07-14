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
var OpenIntentChatbotFactory = require('./chatbot-factory');
var StandaloneSessionManager = require('./session-manager/standalone-driver');
var RedisSessionManager = require('./session-manager/redis-driver');
var Q = require('q');

var NO_CHATBOT_ERROR_MESSAGE = 'No model loaded in the chatbot';

function ChatbotInterface(config) {
    this._chatbot = undefined;
    this._model = undefined;
    this._sessionManagerDriver = undefined;

    this.setModel = function(botmodel) {
        var deferred = Q.defer();
        var userCommandsDriver = undefined;

        if('commands' in botmodel) {
            var userCommandsEntry = botmodel['commands'];
            if('type' in userCommandsEntry && 'script' in userCommandsEntry && userCommandsEntry['type'] == 'js') {
                var VMUserCommandsDriver = require('./user-defined-actions/vm-driver');
                userCommandsDriver = new VMUserCommandsDriver(userCommandsEntry['script']);
            }
            else if('type' in userCommandsEntry && userCommandsEntry['type'] == 'REST') {
                var RestUserCommandsDriver = require('./user-defined-actions/rest-driver');
                userCommandsDriver = new RestUserCommandsDriver();
            }
        }
        else
        {
            deferred.reject('Missing "commands" property.');
            return deferred.promise;
        }


        if('model' in botmodel) {
            var botmodelEntry = botmodel['model'];
            if('json' in botmodelEntry) {
                this._chatbot = OpenIntentChatbotFactory.fromJsonModel(botmodelEntry['json'],
                    this._sessionManagerDriver, userCommandsDriver);
            }
            else if('script' in botmodelEntry && 'dictionary' in botmodelEntry) {
                try {
                    this._chatbot = OpenIntentChatbotFactory.fromOIML(botmodelEntry['dictionary'], botmodelEntry['script'],
                        this._sessionManagerDriver, userCommandsDriver);
                }
                catch(err) {
                    deferred.reject(err);
                    return deferred.promise;
                }
            }
        }
        else {
            deferred.reject('Missing "model" property.');
            return deferred.promise;
        }

        if(this._chatbot) {
            this._model = botmodel;
            deferred.resolve();
        }
        else {
            deferred.reject('Error while instantiating the chatbot');
        }

        return deferred.promise;
    }

    this.getModel = function() {
        var deferred = Q.defer();
        if(this._model) {
            deferred.resolve(this._model);
        }
        else {
            deferred.reject(NO_CHATBOT_ERROR_MESSAGE);
        }
        return deferred.promise;
    }

    this.talk = function(sessionId, message) {
        var deferred = Q.defer();

        if(!sessionId) {
            deferred.reject('No sessionId provided');
            return deferred.promise;
        }

        if(!message) {
            deferred.reject('No message provided');
            return deferred.promise;
        }

        if(!this._chatbot) {
            deferred.reject(NO_CHATBOT_ERROR_MESSAGE);
            return deferred.promise;
        }

        this._chatbot.talk(sessionId, message)
            .then(function(replies) {
                deferred.resolve(replies);
            })
            .fail(function(err) {
                deferred.reject(err);
            })

        return deferred.promise;
    }

    this.setState = function(sessionId, state) {
        var deferred = Q.defer();

        if(!sessionId) {
            deferred.reject('No sessionId provided');
            return deferred.promise;
        }

        if(!state) {
            deferred.reject('No state provided');
            return deferred.promise;
        }

        if(!this._chatbot) {
            deferred.reject(NO_CHATBOT_ERROR_MESSAGE);
            return deferred.promise;
        }

        this._chatbot.setState(sessionId, state)
            .then(function() {
                deferred.resolve();
            })
            .fail(function(err) {
                deferred.reject(err);
            });

        return deferred.promise;
    }

    this.getState = function(sessionId) {
        var deferred = Q.defer();

        if(!sessionId) {
            deferred.reject('No sessionId provided');
            return deferred.promise;
        }

        if(!this._chatbot) {
            deferred.reject(NO_CHATBOT_ERROR_MESSAGE);
            return deferred.promise;
        }

        this._chatbot.getState(sessionId)
            .then(function(state) {
                deferred.resolve(state);
            })
            .fail(function(err) {
                deferred.reject(err);
            });

        return deferred.promise;
    }


    if(config && 'session' in config) {
        var sessionEntry = config['session'];
        if('type' in sessionEntry && 'config' in sessionEntry) {
            if(sessionEntry['type'] == 'redis') {
                redisConfig = sessionEntry['config'];
                var hostname = redisConfig['host'];
                var port = redisConfig['port'];
                this._sessionManagerDriver = new RedisSessionManager(hostname, port);
            }
        }
    }

    if(!this._sessionManagerDriver) {
        this._sessionManagerDriver = new StandaloneSessionManager();
    }

    return this;
}

module.exports = ChatbotInterface;
