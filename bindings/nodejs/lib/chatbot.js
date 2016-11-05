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
var createChatbot = require('./chatbot-api/chatbot');
var Q = require('q');

function ChatbotWithMiddleware() {
    this._chatbot;
    this._middlewares = [];
}


ChatbotWithMiddleware.prototype.set = function(key, middleware) {
  this._middlewares[key] = middleware;
};

ChatbotWithMiddleware.prototype.unset = function(key) {
    delete this._middlewares[key];
};

ChatbotWithMiddleware.prototype.get = function(key) {
    return this._middlewares[key];
};

ChatbotWithMiddleware.prototype.start = function(botmodel, config) {
    var self = this;
    var middlewares = this._middlewares;
    var promises = [];

    return createChatbot(botmodel, config)
    .then(function(chatbot) {
        self._chatbot = chatbot;
        promises.push(self._chatbot.setupUserCommands());
        for(var i in middlewares) {
            promises.push(middlewares[i].start(self._chatbot));
        }
        return Q.all(promises);
    });
};

ChatbotWithMiddleware.prototype.stop = function() {
    var middlewares = this._middlewares;
    var promises = [];
    for(var i in middlewares) {
        promises.push(middlewares[i].stop());
    }
    promises.push(this._chatbot.cleanupUserCommands());
    return Q.all(promises);
};

module.exports = ChatbotWithMiddleware;
