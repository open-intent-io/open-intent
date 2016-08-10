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

var createChatbot = require('../../lib/chatbot');
var Rest = require('../../lib/middleware/rest');

var REST_PORT = 10010;

describe('Test REST middleware', function() {
    var RestMiddleware = undefined;

    var file = path.resolve(__dirname, '../res/food_bot/dictionary.json');
    var dictionary = JSON.parse(fs.readFileSync(file, 'utf-8'));

    var file = path.resolve(__dirname, '../res/food_bot/script.txt');
    var oiml = fs.readFileSync(file, 'utf-8');

    var userCommands = require(path.resolve(__dirname, '../res/food_bot/user_commands.js'));

    before(function(done) {
        var botmodel = {
            dictionary: dictionary,
            oiml: oiml,
            user_commands: userCommands
        };

        createChatbot(botmodel)
        .then(function(chatbot) {
            RestMiddleware = Rest(REST_PORT);
            chatbot.use(RestMiddleware);
            done();
        });
    });

    after(function() {
        RestMiddleware.detach();
    });

    it('should return the state when calling getSate', function(done) {
        request(RestMiddleware._server)
        .get('/state/ABC')
        .set('Accept', 'application/json')
        .expect('Content-Type', /json/)
        .expect(200)
        .end(function(err, res) {
            res.body.should.eql({ 'state': '@root' });
        done();
        });
    });

    it('should return ok status code when calling setSate', function(done) {
        request(RestMiddleware._app )
        .put('/state/ABC')
        .type('json')
        .send({ 'state': '@yesno' })
        .set('Accept', 'application/json')
        .expect('Content-Type', /json/)
        .expect(200)
        .end(function(err, res) {
            res.body.should.eql({ 'message': 'OK' });
            done();
        });
    });

    it('should return a reply when calling talk', function(done) {
        request(RestMiddleware._server)
        .post('/talk/ABC')
        .send({ 'message': 'yes' })
        .set('Accept', 'application/json')
        .expect('Content-Type', /json/)
        .expect(200)
        .end(function(err, res) {
            res.body.should.eql({ 'replies': ['I\'m ordering, it is gonna be 5$.'] });
        done();
        });
    });
});
