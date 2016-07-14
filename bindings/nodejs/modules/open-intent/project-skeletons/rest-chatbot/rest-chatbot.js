
var openintent = require('open-intent');
var path = require('path');
var Q = require('q');

module.exports = RestChatbot;


function RestChatbot() {
    var _this = this;
    this._chatbotServer = undefined;

    this.stop = function() {
        if(_this._chatbotServer) {
            _this._chatbotServer.close();
        }
    }

    this.start = function(port, modelDirectory, withIRCClient) {
        var deferred = Q.defer();

        openintent.ModelBuilder()
            .withDictionaryFromFile(path.join(modelDirectory, 'dictionary.json'))
            .withOIMLFromFile(path.join(modelDirectory, 'script.txt'))
            .withJsUserCommandsFromFile(path.join(modelDirectory, 'user_commands.js'))
            .build(function(err, botmodel) {
                if(err) {
                    console.log('Error:' + err);
                    return 1;
                }

                var config = {
                    port: port,
                    model: botmodel
                }

                openintent.createRestChatbotServer(config)
                .then(function(chatbot) {
                    _this._chatbotServer = chatbot;

                    if(withIRCClient) {
                        console.log(">>> Ready to talk to the chatbot <<<");
                        openintent.createIRCChatbotClient('http://localhost:' + port);
                    }
                    deferred.resolve();
                })
                .fail(function(err) {
                    deferred.reject(err);
                });
            });

        return deferred.promise;
    }

    return this;
}