# open-intent

open-intent npm module allows you to build chatbot in seconds with your own model.
Just type:

    npm init
    npm install --save open-intent

And create an app.js file with the following content

    var openintent = require('open-intent');

    var chatbot = new openintent.Chatbot();

    chatbot.setModel(botmodel)
    .then(function() {

    });