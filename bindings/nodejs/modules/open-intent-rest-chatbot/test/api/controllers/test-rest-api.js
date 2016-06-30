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
var server = require('../../../app');
var fs = require('fs');

var file = 'test/res/chatbot.json';
var jsonModel = fs.readFileSync(file, 'utf-8');

var file = 'test/res/userCommands.js';
var userCommands = fs.readFileSync(file,'utf-8');

describe('controllers', function() {

  describe('rest-api', function() {

    var commands = {
        "type": "js",
        "script": userCommands
    };

    var model = {
        "json": jsonModel
    }

    var botmodel = {
        'commands': commands,
        'model': model
    };

    describe('Call methods while no model loaded', function() {

        it('should return an error when calling getModel', function(done) {

        request(server)
            .get('/model')
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(500)
            .end(function(err, res) {
                res.body.should.eql({ 'message': 'No model loaded in the chatbot' });
                done();
            });
        });

        it('should return an error when calling getSate', function(done) {

        request(server)
            .get('/state/ABC')
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(500)
            .end(function(err, res) {
                res.body.should.eql({ 'message': 'No model loaded in the chatbot' });
                done();
            });
        });

        it('should return an error when calling setSate', function(done) {

        request(server)
            .put('/state/ABC')
            .send({ 'state': 'AState' })
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(500)
            .end(function(err, res) {
                res.body.should.eql({ 'message': 'No model loaded in the chatbot' });
                done();
            });
        });

        it('should return an error when calling talk', function(done) {
        request(server)
            .post('/talk/ABC')
            .send({ 'message': 'Bob' })
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(500)
            .end(function(err, res) {
                res.body.should.eql({ 'message': 'No model loaded in the chatbot' });
                done();
            });
        });
    });

    describe('Given a botmodel', function() {

        it('should return ok when putting a good model', function(done) {
            request(server)
            .put('/model')
            .set('Accept', 'application/json')
            .send(botmodel)
            .expect('Content-Type', /json/)
            .expect(200)
            .end(function(err, res) {
                should.not.exist(err);
                res.body.should.eql({ 'message': 'OK' });
                done();
            });
        });
    });

    describe('When a model has been provided', function() {
        // The model has been set for the next tests

        it('should return the model when calling getModel', function(done) {

            request(server)
            .get('/model')
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(200)
            .end(function(err, res) {
                res.body.should.eql(botmodel);
            done();
            });
        });

        it('should return the state when calling getSate', function(done) {

            request(server)
            .get('/state/ABC')
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(200)
            .end(function(err, res) {
                res.body.should.eql({ 'state': 'init' });
            done();
            });
        });

        it('should return ok status code when calling setSate', function(done) {

            request(server)
            .put('/state/ABC')
            .send({ 'state': 'wait_order' })
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(200)
            .end(function(err, res) {
                res.body.should.eql({ 'message': 'OK' });
            done();
            });
        });

        it('should return a reply when calling talk', function(done) {
            request(server)
            .post('/talk/ABC')
            .send({ 'message': 'Je veux un Coca' })
            .set('Accept', 'application/json')
            .expect('Content-Type', /json/)
            .expect(200)
            .end(function(err, res) {
                res.body.should.eql({ 'replies': ['Vous-voulez quelque chose d\'autre ?'] });
            done();
        });
      });
    });
  });
});
