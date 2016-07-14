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
var IRCChatbotClient = require('../../irc-client');
var RestChatbotServer = require('../../rest-server');
var Q = require('q');

var fs = require('fs');
var path = require('path');

function RestChatbot(config) {
    var deferred = Q.defer();

    var port = 10010;
    if(config && 'port' in config) {
        port = config.port
    }

    var modelDirectory = process.cwd();
    if(config && 'model_directory' in config) {
        modelDirectory = config.model_directory;
    }

    var DICTIONARY_FILE = path.join(modelDirectory, 'dictionary.json');
    var SCRIPT_FILE = path.join(modelDirectory, 'script.txt');
    var USERCOMMANDS_FILE = path.join(modelDirectory, 'user_commands.js');;

    var dictionary = fs.readFileSync(DICTIONARY_FILE, 'utf-8');
    var script = fs.readFileSync(SCRIPT_FILE, 'utf-8');
    var userCommands = fs.readFileSync(USERCOMMANDS_FILE, 'utf-8');

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


    var restChatbotService = new RestChatbotServer({
        'port': port,
        'model': botmodel
    }, function() {
        deferred.resolve(restChatbotService);
    });

    process.on('SIGINT', function() {
        console.log( "\nGracefully shutting down from SIGINT (Ctrl-C)" );
        process.exit(0);
    });

    return deferred.promise;
}

module.exports = RestChatbot;
