
var path = require('path');
var fs = require('fs');
var openintent = require('open-intent');
var Q = require('q');

module.exports = function(middlewares) {
    var deferred = Q.defer();
    var modelDirectory = path.join(__dirname, '..', 'res');

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
        var promises = [];
        for(var i in middlewares) {
            promises.push(chatbot.use(middlewares[i]));
        }

        Q.all(promises)
        .then(function() {
            chatbot.start()
            .then(function() {
                deferred.resolve();
            })
            .fail(function(err) {
                deferred.reject(err);
            })
        })
        .fail(function(err) {
            deferred.reject(err);
        })
    })
    .fail(function(err) {
        deferred.reject(err);
    });

    return deferred.promise;
};