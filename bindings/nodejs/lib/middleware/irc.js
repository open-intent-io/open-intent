
var readline = require('readline');

var SESSION_ID='ABC';

function startIRCClient(chatbot, rl, stdout) {
    var lastPromise;

    rl.prompt();
    rl.on('line', function(line) {
        if(line == 'quit' || line == 'exit') {
            if(lastPromise) {
                lastPromise.then(function() {
                    process.exit(0);
                }).fail(function() {
                    process.exit(0);
                });
            }
            else {
                process.exit(0);
            }
        }

        var promise = chatbot.talk(SESSION_ID, line);
        promise.then(function(replies) {
            var output = '';
            for(var i in replies) {
                output += replies[i] + '\n';
            }
            stdout.write(output);
            rl.prompt();
        })
        .fail(function(response) {
            console.error('Error: ' + JSON.parse(response.body).message);
            rl.prompt();
        });

        lastPromise = promise;
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

module.exports = function(stdio) {
    return new MiddlewareInterface(stdio);
};