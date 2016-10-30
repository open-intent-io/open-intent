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
var should = require('should');
var request = require('supertest');
var fs = require('fs');
var path = require('path');

var Chatbot = require('../../lib/chatbot');
var DocPublisher = require('../../lib/middleware/doc-publisher');

var REST_PORT = 10010;

describe('Test docu publisher middleware', function() {
    var Middleware;
    var chatbot;
    var file = path.resolve(__dirname, '../res/food_bot/dictionary.json');
    var dictionary = JSON.parse(fs.readFileSync(file, 'utf-8'));

    var file = path.resolve(__dirname, '../res/food_bot/script.txt');
    var oiml = fs.readFileSync(file, 'utf-8');

    var userCommands = require(path.resolve(__dirname, '../res/food_bot/user_commands.js'));

    before(function() {
        var botmodel = {
            dictionary: dictionary,
            oiml: oiml,
            user_commands: userCommands
        };

        Middleware = DocPublisher(REST_PORT);
        var middlewares = [];

        middlewares.push(Middleware);

        chatbot = new Chatbot();
        var config = {
            middlewares: middlewares
        };

        return chatbot.start(botmodel, config);
    });

    after(function() {
        return chatbot.stop();
    });

    it('should return the intent story graph in an HTML page', function(done) {
        request(Middleware._server)
        .get('/intent-story')
        .set('Accept', 'application/json')
        .expect('Content-Type', 'text/html')
        .expect(200)
        .end(function(err, res) {
            res.text.should.eql(fs.readFileSync(path.resolve(__dirname, 'test-doc-publisher.expected.html'), 'utf-8'));
            done();
        });
    });

    it('should return static viz script', function(done) {
        request(Middleware._server)
        .get('/static/viz.js')
        .expect('Content-Type', 'application/javascript')
        .expect(200)
        .end(function(err, res) {
            if(!err) {
                done();
            }
            else {
                console.error(err);
            }
        });
    });
});
