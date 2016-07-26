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
var redis = require('redis');

var REDIS_CONTEXT_KEY = 'context';

module.exports = function(redisHost, redisPort) {
    this._redisClient = undefined;

    this.save = function(sessionId, context) {
        var deferred = Q.defer();
        this._redisClient.hset(sessionId, REDIS_CONTEXT_KEY, JSON.stringify(context), function(err) {
            if(err) {
                deferred.reject(err);
            }
            else {
                deferred.resolve();
            }
        });
        return deferred.promise;
    };

    this.load = function(sessionId) {
        var deferred = Q.defer();

        this._redisClient.hget(sessionId, REDIS_CONTEXT_KEY, function(err, data) {
            if(err) {
                deferred.reject(err);
            }
            else {
                deferred.resolve(JSON.parse(data));
            }
        });
        return deferred.promise;
    };

    if(!redisPort) {
        redisPort = 6379;
    }

    if(!redisHost) {
        redisHost = '127.0.0.1';
    }
    
    this._redisClient = redis.createClient(redisPort, redisHost);

    this._redisClient.on("error", function (err) {
        console.error(err);
    });


    return this;
}
