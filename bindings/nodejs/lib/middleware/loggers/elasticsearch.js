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
module.exports = ElasticSearchLogger;

var fs = require('fs');
var path = require('path');
var elasticsearch = require('elasticsearch');
var Q = require('q');

var TYPE = 'logentry';

function initialize(client, indexName) {
    var deferred = Q.defer();
    var mappings = {
        'properties': {
            'data': {
                'type': 'object',
                'properties': {
                    type: { type: 'string'},
                    session_id: { type: 'string'},
                    message: { type: 'string'}
                }
            },
            'timestamp': { type: 'date'}
        }
    };

    var body = {};
    body[TYPE] = mappings;

    client.indices.create({
        index: indexName,
        type: TYPE
    }, function(error, response) {
        client.indices.putMapping({
            index: indexName,
            type: TYPE,
            body: body
        }, function(error, response) {
            if(error) {
                deferred.reject(error);
                return 1;
            }
            deferred.resolve(response);
        });
    });

    return deferred.promise;
}

function ElasticSearchLogger(config) {
    var host = config.host || 'localhost';
    var port = config.port || 9200;
    var indexName = config.index || 'conversations';

    this._client = new elasticsearch.Client({
        host: host + ':' + port
    });

    this.initialize = function() {
        return initialize(this._client, indexName);
    };

    this.log = function(data) {
        this._client.index({
            index: indexName,
            type: TYPE,
            body: {
                'data': data,
                'timestamp': new Date()
            }
        }, function(err, res) {
            if(err) {
                console.trace(err);
                return 1;
            }
        });
    }
}
