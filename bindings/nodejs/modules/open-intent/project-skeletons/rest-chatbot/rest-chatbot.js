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
var openintent = require('open-intent');
var path = require('path');
var Q = require('q');

module.exports = RestChatbot;


function RestChatbot(stdio) {
    var _this = this;
    this._chatbotServer = undefined;

    this.stop = function() {
        if(_this._chatbotServer) {
            _this._chatbotServer.close();
        }
    }

    this.start = function(port, modelDirectory, withIRCClient) {
        var deferred = Q.defer();

        openintent.ModelBuilder()
            .withDictionaryFromFile(path.join(modelDirectory, 'dictionary.json'))
            .withOIMLFromFile(path.join(modelDirectory, 'script.txt'))
            .withJsUserCommandsFromFile(path.join(modelDirectory, 'user_commands.js'))
            .build(function(err, botmodel) {
                if(err) {
                    console.log('Error:' + err);
                    return 1;
                }

                var config = {
                    port: port,
                    model: botmodel
                }

                openintent.createRestChatbotServer(config)
                .then(function(chatbot) {
                    _this._chatbotServer = chatbot;

                    if(withIRCClient) {
                        console.log(">>> Ready to talk to the chatbot <<<");
                        openintent.createIRCChatbotClient('http://localhost:' + port, stdio);
                    }
                    deferred.resolve();
                })
                .fail(function(err) {
                    deferred.reject(err);
                });
            });

        return deferred.promise;
    }

    return this;
}
