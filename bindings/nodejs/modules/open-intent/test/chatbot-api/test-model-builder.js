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