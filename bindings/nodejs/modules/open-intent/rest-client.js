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
var requestify = require("requestify");
var Q = require('q');
var serialize = require('./lib/chatbot-api/model-serializer').serialize;
var deserialize = require('./lib/chatbot-api/model-serializer').deserialize;

var REQUEST_TIMEOUT = 5000;

module.exports = function(uri) {

    this.talk = function(sessionId, message) {
        var deferred = Q.defer();
        var url = uri + '/talk/' + sessionId;

        requestify.post(url,
            { 'message': message }, { 
            'timeout': REQUEST_TIMEOUT,
            'dataType': 'form-url-encoded'
        })
        .then(function(response) {
            var replies = JSON.parse(response.body).replies;
            deferred.resolve(replies);
        })
        .fail(function(error) {
            deferred.reject(error);
        });

        return deferred.promise;
    }

    this.setState = function(sessionId, state) {
        var url = uri + '/state/' + sessionId;
        return requestify.put(url, {
            'state': state,
            'dataType': 'form-url-encoded'
        }, { 'timeout': REQUEST_TIMEOUT });
    }

    this.getState = function(sessionId) {
        var url = uri + '/state/' + sessionId;
        var deferred = Q.defer();

        requestify.get(url,
            { 'timeout': REQUEST_TIMEOUT })
        .then(function(response) {
            deferred.resolve(JSON.parse(response.body).state);
        })
        .fail(function(error) {
            deferred.reject(error);
        });
        return deferred.promise;
    }

    this.setModel = function(botmodel) {
        var url = uri + '/model';
        return requestify.put(url, serialize(botmodel),
            { 'timeout': REQUEST_TIMEOUT });
    }

    this.getModel = function() {
        var url = uri + '/model';
        var deferred = Q.defer();

        requestify.get(url, { 'timeout': REQUEST_TIMEOUT })
        .then(function(response) {
            deferred.resolve(deserialize(JSON.parse(response.body).model));
        })
        .fail(function(error) {
            deferred.reject(error);
        })
        return deferred.promise;
    }

    return this;
}
