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
const EventEmitter = require('events');

function setupEmitter(commandEmitter, setupAction) {
    var deferred = Q.defer();

    var handlerFound = commandEmitter.emit(setupAction, deferred.resolve);

    // We resolve even if the listener has not been found.
    if(!handlerFound) {
        deferred.resolve();
    }
    return deferred.promise;
}

function beginInternal(commandEmitter) {
    return setupEmitter(commandEmitter, '_setup');
}

function endInternal(commandEmitter) {
    return setupEmitter(commandEmitter, '_cleanup');
}

function executeInternal(commandEmitter, actionId, sessionId, intentVariables) {
    var deferred = Q.defer();

    var handlerFound = commandEmitter.emit(actionId, intentVariables, sessionId, deferred.resolve);

    // We resolve even if the listener has not been found.
    if(!handlerFound) {
        deferred.resolve();
    }
    return deferred.promise;
}


module.exports = function(commandHandler) {
    this._emitter = new EventEmitter();

    // Pass the emitter to the command handler
    commandHandler(this._emitter);

    this.setup = function() {
        return beginInternal(this._emitter);
    };

    this.cleanup = function() {
        return endInternal(this._emitter);
    };

    this.execute = function(actionId, sessionId, intentVariables) {
        return executeInternal(this._emitter, actionId, sessionId, intentVariables);
    };

    return this;
};
