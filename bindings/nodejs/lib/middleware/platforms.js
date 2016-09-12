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
const
    crypto = require('crypto'),
    request = require('request'),
    express = require('express'),
    fs = require('fs'),
    path = require('path'),
    Q = require('q');

function deployPlatform(chatbot, platformName, platform, config, app) {
    var selection = config.selection;

    if (selection[platformName]) {
        platform.attach(chatbot, config[platformName], app);
    }
}

function deployPlatforms(chatbot, platforms, config, app) {
    for (var platformName in platforms) {
        if (platforms.hasOwnProperty(platformName)) {
            deployPlatform(chatbot, platformName, platforms[platformName], config, app);
        }
    }
}

function MiddlewareInterface(config) {

    this._platforms = {};
    this._app = undefined;
    this._server = undefined;

    if(config.selection.kik)
        this._platforms['kik'] = require(path.resolve(__dirname, "platforms/controllers/kik"));

    if(config.selection.messenger)
        this._platforms['messenger'] = require(path.resolve(__dirname, "platforms/controllers/messenger"));

    if(config.selection.skype)
        this._platforms['skype'] = require(path.resolve(__dirname, "platforms/controllers/skype"));

    if(config.selection.slack)
        this._platforms['slack'] = require(path.resolve(__dirname, "platforms/controllers/slack"));

    if(config.selection.telegram)
        this._platforms['telegram'] = require(path.resolve(__dirname, "platforms/controllers/telegram"));

    this.attach = function(chatbot) {
        var deferred = Q.defer();
        var app = express();

        app.use(express.static('public'));

        try {
            deployPlatforms(chatbot, this._platforms, config, app);
        }
        catch(err) {
            console.error(err);
            throw Error(err);
        }
        this._app = app;
        deferred.resolve();

        return deferred.promise;
    };

    this.detach = function() {
        if(this._server) {
            this._server.close();
        }
    };

    this.start = function() {
        var deferred = Q.defer();
        var port = process.env.PORT || (config.general.port) || 5000;

        this._server = this._app.listen(port, function() {
            deferred.resolve();
        });
        return deferred.promise;
    }
}


module.exports = function(config) {
    return new MiddlewareInterface(config);
};
