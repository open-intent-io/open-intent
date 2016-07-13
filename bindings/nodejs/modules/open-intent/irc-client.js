var readline = require('readline');
var RestChatbotClient = require('./rest-client');

var SESSION_ID = '12345';

module.exports = function(uri, stdin, stdout) {

    if(!stdin) {
        stdin = process.stdin;
    }
    if(!stdout) {
        stdout = process.stdout;
    }

    var chatbotClient = new RestChatbotClient(uri);

    var rl = readline.createInterface({
        input: stdin,
        output: stdout,
        prompt: '> '
    });

    rl.prompt();

    rl.on('line', function(line) {
        if(line == 'quit') {
            process.exit();
        }

        chatbotClient.talk(SESSION_ID, line)
        .then(function(replies) {
            var output = '';
            for(var i in replies) {
                output += replies[i] + '\n';
            }
            stdout.write(output);
            rl.prompt();
        })
        .fail(function(response) {
            console.error(JSON.parse(response.body).message);
            rl.prompt();
        });
    });
}
