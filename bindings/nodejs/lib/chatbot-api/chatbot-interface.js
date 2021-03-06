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
var Q = require('q');

function treatMessage(chatbot, userCommandsDriver, sessionId, context, msg, deferred, saveContext) {
    var currentState = context._state;
    var intentFound = chatbot.treatMessage(context, msg, function (actionId, intentVariables) {
        if (userCommandsDriver) {
            userCommandsDriver.execute(actionId, sessionId, intentVariables)
                .then(function (userDefinedVariables) {
                    // Must create userDefinedVariables because it is mandatory in the C++ layer.
                    if (!userDefinedVariables) {
                        userDefinedVariables = {};
                    }

                    //let the user override the branching completely, by specifying
                    //the target state and a dynamic reply from the user_command
                    var replies = [];
                    if('_target_state' in userDefinedVariables && '_reply' in userDefinedVariables) {
                        var targetState = userDefinedVariables['_target_state'];
                        var reply = userDefinedVariables['_reply'];

                        context._state = targetState;
                        replies.push(reply);
                    } else {
                        replies = chatbot.prepareReplies(currentState, actionId, intentVariables, userDefinedVariables);
                    }

                    saveContext(context);
                    deferred.resolve(replies);
                });
        }
        else {
            // No user defined variables.
            var replies = chatbot.prepareReplies(currentState, actionId, intentVariables, {});
            deferred.resolve(replies);
        }
    });

    if(!intentFound) {
        deferred.reject('No intent found');
    }
}

function callTreatMessageWithContext(chatbot, sessionManager, userCommandsDriver, sessionId, context, msg, deferred) {
    try {
        var saveContext = (context) => (sessionManager.save(sessionId, context));
        treatMessage(chatbot, userCommandsDriver, sessionId, context, msg, deferred, saveContext);
    }
    catch(err) {
        console.log(err);
        deferred.reject(err);
    }
}

var talkInternal = function(chatbot, sessionManager, userCommandsDriver, sessionId, msg) {
    var deferred = Q.defer();

    sessionManager.load(sessionId)
    .then(function(context) {
        callTreatMessageWithContext(chatbot, sessionManager, userCommandsDriver, sessionId, context, msg, deferred);
    })
    // If the state does not exist in the session manager
    .fail(function(err) {
        var context = {
            '_state': chatbot.getInitialState()
        };
        callTreatMessageWithContext(chatbot, sessionManager, userCommandsDriver, sessionId, context, msg, deferred);
    });
    return deferred.promise;
};

var setStateInternal = function(sessionManager, sessionId, state) {
    var context = {
        _state: state
    };
    return sessionManager.save(sessionId, context);
};

function getStateInternal(sessionManager, sessionId, initialState) {
    var deferred = Q.defer();
    sessionManager.load(sessionId)
    .then(function(context) {
        if('_state' in context) {
            deferred.resolve(context._state);
        }
        else {
            deferred.reject('No state in the context');
        }
    })
    .fail(function() {
        deferred.resolve(initialState);
    });
    return deferred.promise;
}

var getInitialStateInternal = function(chatbot) {
    var defer = Q.defer();
    defer.resolve(chatbot.getInitialState());
    return defer.promise;
};

var getGraphInternal = function(chatbot) {
    var defer = Q.defer();
    defer.resolve(chatbot.getGraph(chatbot));
    return defer.promise;
};

var setupUserCommands = function(userCommands) {
    return userCommands.setup();
};

var cleanupUserCommands = function(userCommands) {
    return userCommands.cleanup();
};

function OpenIntentChatbot(serializableChatbot, sessionManagerDriver, userCommandsDriver) {

    if(!sessionManagerDriver) {
        throw new Error('No session manager driver');
    }

    this._serializableChatbot = serializableChatbot;
    this._userCommandsDriver = userCommandsDriver;
    this._sessionManager = sessionManagerDriver;
}

OpenIntentChatbot.prototype.talk = function(sessionId, msg) {
    return talkInternal(this._serializableChatbot, this._sessionManager, this._userCommandsDriver, sessionId, msg);
};

OpenIntentChatbot.prototype.setState = function(sessionId, state) {
    return setStateInternal(this._sessionManager, sessionId, state);
};

OpenIntentChatbot.prototype.getState = function(sessionId) {
    var initialState = this._serializableChatbot.getInitialState();
    return getStateInternal(this._sessionManager, sessionId, initialState);
};

OpenIntentChatbot.prototype.getInitialState = function() {
    return getInitialStateInternal(this._serializableChatbot);
};

OpenIntentChatbot.prototype.getGraph = function() {
    return getGraphInternal(this._serializableChatbot);
};

OpenIntentChatbot.prototype.setupUserCommands = function() {
    return setupUserCommands(this._userCommandsDriver);
};

OpenIntentChatbot.prototype.cleanupUserCommands = function() {
    return cleanupUserCommands(this._userCommandsDriver);
};



module.exports = OpenIntentChatbot;
