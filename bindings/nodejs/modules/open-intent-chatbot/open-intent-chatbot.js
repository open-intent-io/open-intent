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
var OpenIntent = require('./open-intent.node');

var talkInternal = function(chatbot, userCommandsDriver, sessionManager, req, res) {
    var sessionId = req.body.sessionId;
    var message = req.body.msg;

    sessionManager.load(sessionId, function(err, ctx) {
        var context = {};

        if (!ctx) {
            context = {
                _state: "",
                _userDefinedVariables: {}
            };
        }
        else {
            context = JSON.parse(ctx);
        }

        chatbot.treatMessage(context, message, function(actionId, intentVariables) {
            userCommandsDriver(actionId, sessionId, intentVariables, function(userDefinedVariables) {
                // Must create userDefinedVariables because it is mandatory in the C++ layer.
                if(!userDefinedVariables) {
                    userDefinedVariables = {};
                }

                var replies = chatbot.prepareReplies(actionId, intentVariables, userDefinedVariables);
                res.send(JSON.stringify(replies));
            });
        });

        sessionManager.save(sessionId, JSON.stringify(context));
    });
}

var setStateInternal = function(sessionManager, req, res) {
    var context = {
        _state: req.body.state,
        _userDefinedVariables: {}
    };

    sessionManager.save(req.body.sessionId, JSON.stringify(context), function(error) {
        if(error) {
            res.send('{ "code": "NOK" }');
        }
        else {
            res.send('{ "code": "OK" }');
        }
    });
}

function getStateInternal(sessionId, req, res) {

    sessionManager.load(sessionId, function(err, ctx) {
        var context = {};

        if (!ctx) {
            res.send('{ "code": "NOK" }');
        }
        else {
            context = JSON.parse(ctx);
            res.send('{ "code": "OK", "state": "' + context._state + '" }');
        }
    });
}

var getInitialStateInternal = function(chatbot) {
    return chatbot.getInitialState();
}

var getTerminalStatesInternal = function(chatbot) {
    return chatbot.getTerminalStates();
}

function OpenIntentChatbot(serializableChatbot, userCommandsDriver, sessionManagerDriver) {

    var _this = this;
    this._serializableChatbot = serializableChatbot;
    this._userCommandsDriver = userCommandsDriver;
    this._sessionManager = sessionManagerDriver;

    this.talk = (req, fn) => talkInternal(_this._serializableChatbot, _this._userCommandsDriver, _this._sessionManager, req, fn);
    this.setState = (req, fn) => setStateInternal(_this._sessionManager, req, fn);
    this.getState = (req, fn) => getStateInternal(_this._sessionManager, req, fn);
    this.getInitialState = () => getInitialStateInternal(_this._serializableChatbot);
    this.getTerminalStates = () => getTerminalStatesInternal(_this._serializableChatbot);
}


function createFromOIML(dictionaryModel, openIntentMLModel, userCommandsDriver, sessionManagerDriver) {

    if(!sessionManagerDriver) {
        var redisDriver = require('./session-manager/redis-driver');
        sessionManagerDriver = redisDriver();
    }

    var openIntentChatbot = new OpenIntentChatbot(OpenIntent.createSerializableChatbotFromOIML(dictionaryModel, openIntentMLModel),
        userCommandsDriver, sessionManagerDriver);
    return openIntentChatbot;
}


function createFromJsonModel(jsonModel, userCommandsDriver, sessionManagerDriver) {

    if(!sessionManagerDriver) {
        var redisDriver = require('./session-manager/redis-driver');
        sessionManagerDriver = redisDriver();
    }

    var openIntentChatbot = new OpenIntentChatbot(OpenIntent.createSerializableChatbotFromJsonModel(jsonModel),
        userCommandsDriver, sessionManagerDriver);
    return openIntentChatbot;
}

module.exports.fromOIML = createFromOIML;
module.exports.fromJsonModel = createFromJsonModel;
