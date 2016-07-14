var should = require('should');
var util = require('util');
var config = require('../../config/config');
var path = require('path');
var proxyquire =  require('proxyquire').noPreserveCache();
var tmp = require('tmp');
var fs = require('fs');
var _ = require('lodash');

/*
 create: create,
 start: start,
 */

describe('test project commands', function() {

    var tmpDir;
    var initialDir;
    var spawn = {};

    before(function (done) {
        initialDir = process.cwd();
        tmp.setGracefulCleanup();

        // set up project dir
        tmp.dir({unsafeCleanup: true}, function (err, path) {
            should.not.exist(err);
            tmpDir = path;
            process.chdir(tmpDir);
            done();
        });
    });

    after(function() {
        process.chdir(initialDir);
    });

    var nodemonOpts = {};
    var projectStubs = {
        'child_process': {
            spawn: function (command, args, options) {
                spawn.command = command;
                spawn.args = args;
                spawn.options = options;

                var ret = {};
                ret.stdout = {
                    on: function () {
                    }
                };
                ret.stderr = {
                    on: function () {
                    }
                };
                ret.on = function (name, cb) {
                    if (name === 'close') {
                        setTimeout(function () {
                            cb(0);
                        }, 0);
                    }
                    return ret;
                };
                return ret;
            }
        },
        'nodemon': {
            on: function (name, cb) {
                if (name === 'start') {
                    setTimeout(function () {
                        cb();
                        nodemonOpts.cb();
                    }, 0);
                }
                return this;
            },
            _init: function (opts, cb) {
                nodemonOpts = opts;
                nodemonOpts.cb = cb;
            },
            '@noCallThru': true
        }
    };
    var project = proxyquire('../../lib/project/project', projectStubs);

    describe('create', function () {

        it('should err if project directory already exists', function (done) {
            var name = 'create_err';
            var projPath = path.resolve(tmpDir, name);
            fs.mkdirSync(projPath);
            process.chdir(tmpDir);
            project.create(name, {}, function (err) {
                should.exist(err);
                done();
            });
        });

        it('should create a new connect project', function (done) {
            var name = 'create';
            var projPath = path.resolve(tmpDir, name);
            process.chdir(tmpDir);
            project.create(name, undefined, function (err) {
                should.not.exist(err);
                // check a couple of files
                var packageJson = path.resolve(projPath, 'package.json');
                fs.existsSync(packageJson).should.be.ok;
                fs.existsSync(path.resolve(projPath, 'node_modules')).should.not.be.ok;
                fs.existsSync(path.resolve(projPath, '.gitignore')).should.be.ok;

                // check spawn `npm install`
                spawn.command.should.equal('npm');
                spawn.args.should.containEql('install');
                spawn.options.should.have.property('cwd', fs.realpathSync(projPath));

                // check package.json customization
                fs.readFile(packageJson, {encoding: 'utf8'}, function (err, string) {
                    if (err) {
                        return cb(err);
                    }
                    var project = JSON.parse(string);
                    project.name.should.equal(name);
                    done();
                });
            });
        });
    });

   describe('start', function () {

        var name = 'start';
        var projPath;

        before(function (done) {
            projPath = path.resolve(tmpDir, name);
            process.chdir(tmpDir);
            project.create(name, {framework: 'connect'}, done);
        });

        it('should pass debug options', function (done) {
            var options = {debug: 'true,test'};
            project.start(projPath, options, function (err) {
                should.not.exist(err);
                nodemonOpts.nodeArgs.should.containDeep(['--debug=' + options.debug]);
                done();
            });
        });

        it('should start in debug break mode', function (done) {
            var options = {debugBrk: true};
            project.start(projPath, options, function (err) {
                should.not.exist(err);
                nodemonOpts.nodeArgs.should.containDeep(['--debug-brk']);
                done();
            });
        });

        it('should pass extra arguments', function (done) {
            var options = {nodeArgs: '--harmony'};
            project.start(projPath, options, function (err) {
                should.not.exist(err);
                nodemonOpts.nodeArgs.should.containDeep(['--harmony']);
                done();
            });
        });

        it('should pass multiple extra arguments separately', function (done) {
            var options = {nodeArgs: '--harmony --harmony_destructuring'};
            project.start(projPath, options, function (err) {
                should.not.exist(err);
                nodemonOpts.nodeArgs.should.containDeep(['--harmony', '--harmony_destructuring']);
                done();
            });
        });


        it('should combine extra arguments with debug', function (done) {
            var options = {debug: true, nodeArgs: '--harmony'};
            project.start(projPath, options, function (err) {
                should.not.exist(err);
                nodemonOpts.nodeArgs.should.containDeep(['--debug', '--harmony']);
                done();
            });
        });
    });
});