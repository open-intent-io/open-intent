
var path = require('path');
var fs = require('fs');
var openintent = require('open-intent');

module.exports = function(middlewares, fn) {

    var modelDirectory = path.join(__dirname, 'res');

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
        for(var i=0; i<middlewares.length; ++i) {
            chatbot.use(middlewares[i]);
        }
        fn();
    })
    .fail(function(err) {
        fn(error);
        console.error('Error:', err);
    })
};