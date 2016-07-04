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
var ChatbotService = require('open-intent').RestChatbotServer;
var ChatbotClient = require('open-intent').RestChatbotClient;

var fs = require('fs');

var DEBUG = process.env.DEBUG;
var SERVICE_HOST = process.env.SERVICE_HOST || 'http://127.0.0.1' 
var SERVICE_PORT = process.env.SERVICE_PORT || 10010;

var DICTIONARY_FILE = 'res/dictionary.json';
var SCRIPT_FILE = 'res/script.txt';
var USERCOMMANDS_FILE = 'res/user_commands.js';

var dictionary = fs.readFileSync(DICTIONARY_FILE, 'utf-8');
var script = fs.readFileSync(SCRIPT_FILE, 'utf-8');
var userCommands = fs.readFileSync(USERCOMMANDS_FILE, 'utf-8');

var host = SERVICE_HOST + ':' + SERVICE_PORT;

var botmodel = {
    'model': {
        'script': script,
        'dictionary': dictionary
    },
    'commands': {
        'type': 'js',
        'script': userCommands
    }
}

function example(stdin, stdout, done) {
    var restChatbotService = new ChatbotService(SERVICE_PORT, function() {
        var restClient = new ChatbotClient(host);
        
        restClient.setModel(botmodel)
        .then(function(response) {
            if(DEBUG == 'true') {
                var TerminalClient = require('./terminal-client');
                var terminal = TerminalClient(restClient, stdin, stdout);
                done();
            }
        })
        .fail(function(err) {
            console.error(err);
        });
    });

    process.on('SIGINT', function() {
        console.log( "\nGracefully shutting down from SIGINT (Ctrl-C)" );
        process.exit(0);
    });

    return restChatbotService;
}

module.exports = example;
