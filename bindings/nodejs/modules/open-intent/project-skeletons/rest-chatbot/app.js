
var path = require('path');
var openintent = require('open-intent');

var model_directory = path.join(__dirname, 'res');
var SERVICE_PORT = 8080;

var withIRC = true;

openintent.ModelBuilder()
    .withDictionaryFromFile(path.join(model_directory, 'dictionary.json'))
    .withOIMLFromFile(path.join(model_directory, 'script.txt'))
    .withUserCommandsFromFile(path.join(model_directory, 'user_commands.js'))
    .build(function(botmodel) {
        var config = {
            port: SERVICE_PORT,
            model: botmodel
        }

        openintent.createRestChatbotServer(config)
        .then(function(chatbot) {
            if(withIRC) {
                console.log(">>> Ready to talk to the chatbot <<<");
                openintent.IRCChatbotClient('http://localhost:' + SERVICE_PORT);
            }
        });
    });
