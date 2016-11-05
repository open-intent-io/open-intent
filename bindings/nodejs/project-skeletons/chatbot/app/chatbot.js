
var path = require('path');
var fs = require('fs');
var openintent = require('open-intent');

function Chatbot() {
    this._chatbot = new openintent.Chatbot();
}

Chatbot.prototype.set = function(key, middleware) {
    this._chatbot.set(key, middleware);
};

Chatbot.prototype.get = function(key) {
    return this._chatbot.get(key);
};


Chatbot.prototype.start = function() {
    var modelDirectory = path.join(__dirname, '..', 'res');

    var dictionaryFilepath = path.join(modelDirectory, 'dictionary.json');
    var oimlFilepath = path.join(modelDirectory, 'script.txt');
    var userCommandsFilepath = path.join(modelDirectory, 'user_commands.js');

    var botmodel = {
        dictionary: JSON.parse(fs.readFileSync(dictionaryFilepath, 'utf-8')),
        oiml: fs.readFileSync(oimlFilepath, 'utf-8'),
        user_commands: require(userCommandsFilepath)
    };

    return this._chatbot.start(botmodel);
};

Chatbot.prototype.stop = function() {
    return this._chatbot.stop();
};

module.exports = Chatbot;