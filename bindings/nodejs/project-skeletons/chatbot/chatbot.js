
var path = require('path');
var fs = require('fs');

module.exports = function(stdio, fn) {

    var modelDirectory = path.join(__dirname, 'res');
    var REST_PORT = 8080;

    var openintent = require('open-intent');

    var dictionaryFilepath = path.join(modelDirectory, 'dictionary.json');
    var oimlFilepath = path.join(modelDirectory, 'script.txt');
    var userCommandsFilepath = path.join(modelDirectory, 'user_commands.js');

    var botmodel = {
        dictionary: JSON.parse(fs.readFileSync(dictionaryFilepath, 'utf-8')),
        oiml: fs.readFileSync(oimlFilepath, 'utf-8'),
        user_commands: require(userCommandsFilepath)
    };

    openintent.createChatbot(botmodel)
    .then(function(chatbot) {
        chatbot.use(openintent.middleware.irc(stdio));
        chatbot.use(openintent.middleware.rest(REST_PORT));
        //chatbot.use(openintent.middleware.platforms());
        fn();
    })
    .fail(function(err) {
        fn(error);
        console.error('Error:', err);
    })
};