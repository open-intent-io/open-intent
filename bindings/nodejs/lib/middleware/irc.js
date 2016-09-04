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
module.exports = function(stdio) {
    return new MiddlewareInterface(stdio);
};


var readline = require('readline');
var seqqueue = require('seq-queue');
var Q = require('q');

var SESSION_ID='ABC';

function buildTalkTask(chatbot, rl, stdout, message) {
    return function(task) {
        chatbot.talk(SESSION_ID, message)
        .then(function(replies) {
            var output = '';
            for(var i in replies) {
                output += replies[i] + '\n';
            }
            stdout.write(output);
            rl.prompt();
            task.done();
        })
        .fail(function(response) {
            console.error('Error: ' + JSON.parse(response.body).message);
            rl.prompt();
            task.done();
        });
    };
}

function exitCallback() {
    process.exit(0);
}

function startIRCClient(chatbot, rl, stdout) {
    var deferred = Q.defer();
    var queue = seqqueue.createQueue();

    rl.prompt();
    rl.on('line', function(line) {
        if(line == 'quit' || line == 'exit') {
            queue.push(exitCallback);
        }
        queue.push(buildTalkTask(chatbot, rl, stdout, line));
    });

    deferred.resolve();
    return deferred.promise;
}



function MiddlewareInterface(stdio) {
    var _this = this;

    if(!stdio) {
        stdio = {
            stdin: process.stdin,
            stdout: process.stdout
        };
    }

    this.attach = function(chatbot) {
        var deferred = Q.defer();
        _this._chatbot = chatbot;
        _this._readline = readline.createInterface({
            input: stdio.stdin,
            output: stdio.stdout,
            prompt: '> '
        });
        deferred.resolve();
        return deferred.promise;
    };

    this.detach = function() {
    };

    this.start = function() {
        return startIRCClient(_this._chatbot, _this._readline, stdio.stdout);
    };
}
