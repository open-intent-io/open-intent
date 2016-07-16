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
