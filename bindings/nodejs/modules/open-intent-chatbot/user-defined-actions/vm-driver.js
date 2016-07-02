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
var vm = require('vm');
var Q = require('q');

function runUserCommandsInVM(userCommands, actionId, sessionId, intentVariables) {
    var deferred = Q.defer();

    var context = {
        'sessionId': sessionId,
        'intentVariables': intentVariables,
        'next': deferred.resolve,
        'userCommands': userCommands,
        'actionId': actionId,
        'requireFromString': require('require-from-string')
    }

    var sandbox = vm.createContext(context);
    vm.createContext(sandbox);
    vm.runInContext(" \
var userCommandsDict = requireFromString(userCommands); \
if(actionId in userCommandsDict) { \
    userCommandsDict[actionId](intentVariables, sessionId, next); \
} else { \
    next() \
} \
", sandbox, { 'timeout': 2000 });
    return deferred.promise;
}


module.exports = function(userCommands) {

    if (!(userCommands !== null && typeof userCommands == 'string')) {
        throw new Error('Bad user commands (must be a string).');
    }

    this._userCommands = userCommands;

    this.execute = function(actionId, sessionId, intentVariables) {
        return runUserCommandsInVM(this._userCommands, actionId, sessionId, intentVariables);
    }

    return this;
}
