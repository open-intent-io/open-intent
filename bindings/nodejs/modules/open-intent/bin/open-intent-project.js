var app = require('commander');
var util = require('util');
var _ = require('lodash');

var config = require('../config/config');
var project = require('../lib/project/project')


function execute(command, header) {
    var cb = function(err, reply) {
        if (header && !err) {
            print(header);
            print(Array(header.length + 1).join('='));
        }
        if (!reply && !err) { reply = 'done'; }
        printAndExit(err, reply);
    };
    return function() {
        try {
            var args = Array.prototype.slice.call(arguments);
            args.push(cb);
            if (!command) {
                return cb(new Error('missing command method'));
            }
            if (args.length !== command.length) {
                return cb(new Error('incorrect arguments'));
            }
            var reply = command.apply(this, args);
            if (reply) {
                cb(null, reply);
            }
        } catch (err) {
            cb(err);
        }
    }
}

function print(object) {
    if (util.isError(object)) {
        console.log(config.debug ? object.stack : object);
    } else if (_.isObject(object)) {
        if (object.password) {
            object.password = '******';
        }
        console.log(yaml.safeDump(object, { indent: 2 }));
    } else if (object !== null && object !== undefined) {
        console.log(object);
    } else {
        console.log();
    }
}

function printAndExit(err, output, code) {
    if (err) {
        print(err);
        code = code || 1;
    } else if (output !== null && output !== undefined) {
        print(output);
    }
    process.exit(code || 0);
}

app
    .command('create [name]')
    .description('Create a folder containing an open-intent REST chatbot')
    .action(execute(project.create));

app
    .command('start [directory]')
    .description('Start the open-intent REST chatbot')
    .option('-d, --debug [port]', 'start in remote debug mode')
    .option('-b, --debug-brk [port]', 'start in remote debug mode, wait for debugger connect')
    .option('-n, --node-args <args>', 'run node with extra arguments (like --node-args \"--test\")')
    .action(execute(project.start));


app.parse(process.argv);