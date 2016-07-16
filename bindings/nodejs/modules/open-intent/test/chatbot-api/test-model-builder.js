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
var ModelBuilder = require('../../lib/chatbot-api/model-builder');
var should = require('should');
var fs = require('fs');

describe('Test model builder', function() {

    var dictionaryFile = 'test/chatbot-api/res/food_bot/dictionary.json';
    var userCommandsFile = 'test/chatbot-api/res/food_bot/user_commands.js';
    var scriptFile = 'test/chatbot-api/res/food_bot/script.txt';

    it('should build a model from files with js user commands script', function(done) {
        ModelBuilder()
            .withDictionaryFromFile(dictionaryFile)
            .withJsUserCommandsFromFile(userCommandsFile)
            .withOIMLFromFile(scriptFile)
            .build(function(err, botmodel) {
                should.equal(botmodel.oiml(), fs.readFileSync(scriptFile, 'utf-8'));
                should.equal(botmodel.commands(), fs.readFileSync(userCommandsFile, 'utf-8'));
                should.equal(botmodel.hasJsCommands(), true);
                should.equal(botmodel.dictionary(), fs.readFileSync(dictionaryFile, 'utf-8'));
                done();
            });
    });

    it('should build a model from files with REST user commands script', function(done) {
        ModelBuilder()
            .withDictionaryFromFile(dictionaryFile)
            .withRestUserCommandsFromFile(userCommandsFile)
            .withOIMLFromFile(scriptFile)
            .build(function(err, botmodel) {
                should.equal(botmodel.oiml(), fs.readFileSync(scriptFile, 'utf-8'));
                should.equal(botmodel.commands(), fs.readFileSync(userCommandsFile, 'utf-8'));
                should.equal(botmodel.hasRestCommands(), true);
                should.equal(botmodel.dictionary(), fs.readFileSync(dictionaryFile, 'utf-8'));
                done();
            });
    });

    it('should return an error when dictionary is missing', function(done) {
        ModelBuilder()
            .withRestUserCommandsFromFile(userCommandsFile)
            .withOIMLFromFile(scriptFile)
            .build(function(err, botmodel) {
                should.exist(err);
                done();
            });
    });

    it('should return an error when OIML is missing', function(done) {
        ModelBuilder()
            .withDictionaryFromFile(dictionaryFile)
            .withRestUserCommandsFromFile(userCommandsFile)
            .build(function(err, botmodel) {
                should.exist(err);
                done();
            });
    });
})
