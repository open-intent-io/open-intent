
module.exports = function(stdio) {
    return new MiddlewareInterface(stdio);
};


var readline = require('readline');
var seqqueue = require('seq-queue');

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
    var queue = seqqueue.createQueue();

    rl.prompt();
    rl.on('line', function(line) {
        if(line == 'quit' || line == 'exit') {
            queue.push(exitCallback);
        }
        queue.push(buildTalkTask(chatbot, rl, stdout, line));
    });
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
        _this._readline = readline.createInterface({
            input: stdio.stdin,
            output: stdio.stdout,
            prompt: '> '
        });
        startIRCClient(chatbot, _this._readline, stdio.stdout);
    };

    this.detach = function() {
    };
}
