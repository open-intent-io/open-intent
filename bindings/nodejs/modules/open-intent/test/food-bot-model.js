
var ModelBuilder = require('../lib/chatbot-api/model-builder');
var fs = require('fs');

var dictionaryFile = 'test/res/food_bot/dictionary.json';
var userCommandsFile = 'test/res/food_bot/user_commands.js';
var scriptFile = 'test/res/food_bot/script.txt';

var botmodel = ModelBuilder()
    .withDictionaryFromFile(dictionaryFile)
    .withJsUserCommandsFromFile(userCommandsFile)
    .withOIMLFromFile(scriptFile)
    .build();

module.exports = botmodel;