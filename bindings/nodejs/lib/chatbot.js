
var createChatbot = require('./chatbot-api/chatbot');
var Q = require('q');

function ChatbotInterface(chatbot) {
    this._chatbot = chatbot;
    this._middlewares = [];

    this.use = function(middleware) {
        middleware.attach(this._chatbot);
        this._middlewares.push(middleware);
    };

    return this;
}

function createChatbotInternal(botmodel, config) {
    var deferred = Q.defer();

    createChatbot(botmodel, config)
    .then(function(chatbot) {
        var chatbotInterface = new ChatbotInterface(chatbot);
        deferred.resolve(chatbotInterface);
    })
    .fail(function(err) {
        deferred.reject(err);
    });

    return deferred.promise;
}

module.exports = createChatbotInternal;