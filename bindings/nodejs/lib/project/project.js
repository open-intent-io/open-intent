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
var util = require('util');
var path = require('path');
var fs = require('fs-extra');
var _ = require('lodash');
var readline = require('readline-sync');

var config = require('../../config/config');

module.exports = {
    create: create,
    start: start,
    configure: configure
};

function create(name, options, cb) {
    function validateName(name) {
        var targetDir = path.resolve(process.cwd(), name);
        if (fs.existsSync(targetDir)) {
            return 'Directory ' + targetDir + ' already exists.';
        }
        return true;
    }

    if (name) {
        var valid = validateName(name);
        if (typeof valid === 'string') { return cb(new Error(valid)); }
    }

    var targetDir = path.resolve(process.cwd(), name);

    console.log(util.format('Chatbot %s created in %s', name, targetDir));
    cloneSkeleton(name, targetDir, function() {
        var message = util.format('Success! You may start your new app by running: "open-intent project start %s"', name);
        
        if(options.npm) {
            installDependencies(targetDir, message, cb);
        }
    });
}


function start(directory, options, cb) {

    readProject(directory, options, function(err, project) {
        if (err) { throw err; }

        var fullPath = path.join(project.dirname, project.api.main);
        console.log('Starting: %s...', fullPath);
        var nodemonOpts = {
            script: project.api.main,
            ext: 'js,json,txt',
            nodeArgs: [],
            stdin: true,
            stdout: false,
            restartable: true
        };
        if (project.dirname) { nodemonOpts.cwd = project.dirname; }
        if (options.debugBrk) {
            var debugBrkArg = '--debug-brk';
            if (typeof options.debugBrk === 'string') {
                debugBrkArg += '=' + options.debugBrk;
            }
            nodemonOpts.nodeArgs.push(debugBrkArg);
        }
        if (options.debug) {
            var debugArg = '--debug';
            if (typeof options.debug === 'string') {
                debugArg += '=' + options.debug;
            }
            nodemonOpts.nodeArgs.push(debugArg);
        }
        if (options.nodeArgs) {
            nodemonOpts.nodeArgs = nodemonOpts.nodeArgs.concat(options.nodeArgs.split(' '));
        }

        var nodemon = require('nodemon');

        // hack to enable proxyquire stub for testing...
        if (_.isFunction(nodemon)) {
            nodemon(nodemonOpts);
        } else {
            nodemon._init(nodemonOpts, cb);
        }
        nodemon.on('start', function () {
            console.log('  project will restart on model changes.');

            if (options.open) {
                setTimeout(function() {
                    open(directory, options, cb);
                }, 500);
            }
        }).on('restart', function (files) {
            console.log('Project restarted. Files changed: ' + files);
        }).on('quit', function () {
            process.exit(0);
        }).on('readable', function() { // the `readable` event indicates that data is ready to pick up
            this.stdout.pipe(process.stdout);
            this.stderr.pipe(process.stderr);
        });
    });
}

function askParam(param, configObj) {
    var request = util.format("Please input %s : ", param);
    var answer = readline.question(request);
    configObj[param] = answer;
}

function configureObject(configObj, filename) {
    for (var param in configObj) {
        if (configObj.hasOwnProperty(param)) {
           askParam(param, configObj);
        }
    }
    
    console.log("writing in file : "+filename);
    fs.writeJsonSync(filename, configObj);
}

function configure(directory, options, cb) {

    var config = require(path.resolve(directory, 'app/config.js'));
    var selection = config.selection;

    for (var param in selection) {
        if (selection.hasOwnProperty(param)) {
            if (selection[param])
            {
                var request = util.format("Would you like to configure %s? (y/n) : ", param);
                var answer = readline.question(request);
                if (answer.toLowerCase() === 'y')
                {
                    var filename = path.resolve(directory, 'config/'+param+'/default.json');
                    configureObject(config[param], filename);
                    console.log(util.format("%s configured"), param);
                }    
            }    
        }
    }
    cb();
}

function readProject(directory, options, cb) {

    findProjectFile(directory, options, function(err, fileName) {
        if (err) { return cb(err); }

        var string = fs.readFileSync(fileName, { encoding: 'utf8' });
        var project = JSON.parse(string);

        project.filename = fileName;
        project.dirname = path.dirname(fileName);

        if (!project.api) { project.api = {}; }

        project.api.name = project.name;
        project.api.main = project.main;

        cb(null, project);
    });
}

function findProjectFile(startDir, options, cb) {

    var parent = startDir = startDir || process.cwd();
    var maxDepth = 50;
    var current = null;
    while (current !== parent && maxDepth-- > 0) {
        current = parent;
        var projectFile = path.resolve(current, 'package.json');
        if (fs.existsSync(projectFile)) {
            return cb(null, projectFile);
        }
        parent = path.join(current, '..');
    }
    cb(new Error('Project root not found in or above: ' + startDir));
}




function installDependencies(directory, message, cb) {
    spawn('npm', ['install'], directory, function(err) {
        if (err) {
            console.log(util.format('"npm install" failed. Please run "npm install" in %s.', directory));
            return cb(err);
        }
        cb(null, message);
    });
}

function cloneSkeleton(name, destDir, cb) {

    var skeletonsDir = config.project.skeletonsDir;

    var sourceDir = path.resolve(skeletonsDir, 'chatbot');

    var done = function(err) {
        if (err) { return cb(err); }
        customizeClonedFiles(name, destDir, cb);
    };

    fs.copy(sourceDir, destDir, true, function(err) {
        if (err) { return cb(err); }
        done();
    });
}

function customizeClonedFiles(name, destDir, cb) {
    var rmdir = require('rmdir');

    // npm renames .gitignore to .npmignore, change it back
    var npmignore = path.resolve(destDir, '.npmignore');
    var gitignore = path.resolve(destDir, '.gitignore');
    var dockerDir = path.resolve(destDir, 'docker');

    rmdir(dockerDir, function(err) {
        if(err) { return cb(err); }

        fs.rename(npmignore, gitignore, function(err) {
            if (err && !fs.existsSync(gitignore)) { return cb(err); }

            // rewrite package.json
            var fileName = path.resolve(destDir, 'package.json');
            fs.readFile(fileName, { encoding: 'utf8' }, function(err, string) {
                if (err) { return cb(err); }

                var project = JSON.parse(string);
                project.name = name;

                fs.writeFile(fileName, JSON.stringify(project, null, '  '), cb);
            });
        });
    });
}

function spawn(command, options, cwd, cb) {

    var cp = require('child_process');
    var os = require('os');

    var isWin = /^win/.test(os.platform());

    console.log(util.format('Running "%s %s"...', command, options.join(' ')));

    var npm = cp.spawn(isWin ?
            process.env.comspec :
            command,
        isWin ?
            ['/c'].concat(command, options) :
            options,
        { cwd: cwd });
    npm.stdout.on('data', function (data) {
        console.log(''+data);
    });
    npm.stderr.on('data', function(data) {
        console.log(util.format('%s', data));
    });
    npm.on('close', function(exitCode) {
        if (exitCode !== 0) { var err = new Error('exit code: ' + exitCode); }
        cb(err);
    });
    npm.on('error', function(err) {
        cb(err);
    });
}
