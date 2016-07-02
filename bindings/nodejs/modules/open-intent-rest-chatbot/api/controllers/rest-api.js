/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

The MIT License (MIT)
Copyright (c) 2016 - Clement Michaud, Sergei Kireev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
var chatbot = require('./chatbot-instance');


module.exports = {
    talk: talk,
    setstate: setstate,
    getstate: getstate,
    setmodel: setmodel,
    getmodel: getmodel
};


function talk(req, res) {
    var sessionId = req.swagger.params.sessionId.value;
    var message = req.swagger.params.message.value;

    chatbot.talk(sessionId, message)
    .then(function(replies) {
        res.json({ 'replies': replies });
    }).fail(function(err) {
        res.status(500).send({'message': err});
    });
}


function setstate(req, res) {
    var sessionId = req.swagger.params.sessionId.value;
    var state = req.swagger.params.state.value;

    chatbot.setState(sessionId, state)
    .then(function(state) {
        res.json({ 'message': 'OK' });
    })
    .fail(function(err) {
        res.status(500).send({'message': err});
    });
}

function getstate(req, res) {
    var sessionId = req.swagger.params.sessionId.value;

    chatbot.getState(sessionId)
    .then(function(state) {
        res.json({ 'state': state });
    })
    .fail(function(err) {
        res.status(500).send({'message': err});
    });
}

function setmodel(req, res) {
    var botmodel = req.swagger.params.botmodel.value;

    chatbot.setModel(botmodel)
    .then(function() {
        res.json({ 'message': 'OK' });
    })
    .fail(function(err) {
        res.status(500).send({'message': err});
    });
}

function getmodel(req, res) {

    chatbot.getModel()
    .then(function(model) {
        res.json(model);
    })
    .fail(function(err) {
        res.status(500).send({'message': err});
    });
}
