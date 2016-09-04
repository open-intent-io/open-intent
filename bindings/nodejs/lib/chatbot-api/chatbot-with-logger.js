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
var ChatbotInterface = require('./chatbot-interface');
var util = require('util');
var Q = require('q');

function ChatbotWithLogger(chatbot) {
    this._logger = undefined;

    ChatbotInterface.call(this, chatbot);

    this.talk = function(sessionId, message) {
        var deferred = Q.defer();
        var logger = this._logger;

        if (logger) {
            logger.log({
                'sessionId': sessionId,
                'message': message,
                'type': 'intent'
            });
        }

        this._chatbot.talk(sessionId, message)
        .then(function (replies) {
            if (logger) {
                for(var i in replies) {
                    logger.log({
                        'session_id': sessionId,
                        'message': replies[i],
                        'type': 'replies'
                    });
                }
            }
            deferred.resolve(replies);
        })
        .fail(function (err) {
            deferred.reject(err);
        });

        return deferred.promise;
    };

    this.setLogger = function(logger) {
        this._logger = logger;
    };
}

util.inherits(ChatbotWithLogger, ChatbotInterface);

module.exports = ChatbotWithLogger;
