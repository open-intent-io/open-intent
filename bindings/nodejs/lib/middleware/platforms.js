
function MiddlewareInterface() {

    this.attach = function(chatbot) {

    };

    this.detach = function() {

    };
};


module.exports = function() {
    return new MiddlewareInterface();
};