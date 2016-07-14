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
'use strict';
var OpenIntentChatbot = require('./lib/chatbot-api/chatbot');
var Q = require('q');

module.exports = function(config, ready) {
    var deferred = Q.defer();
    deferred.resolve(new RestChatbotServer(config, ready));
    return deferred.promise;
}


function RestChatbotServer(config, ready) {
    var chatbot = new OpenIntentChatbot();
    var swaggerConfig = {
        appRoot: __dirname // required config
    };

    process.env.SUPPRESS_NO_CONFIG_WARNING = 'true';
    var SwaggerExpress = require('swagger-express-mw');
    var SwaggerUi = require('swagger-tools/middleware/swagger-ui');
    var express = require('express');

    var _this = this;
    _this._app = new express();
    _this._server = undefined;
    _this._app.set('chatbot', chatbot);

    SwaggerExpress.create(swaggerConfig, function(err, swaggerExpress) {
        if (err) { throw err; }

        // Add swagger-ui (This must be before swaggerExpress.register)
        _this._app.use(SwaggerUi(swaggerExpress.runner.swagger));

        // install middleware
        swaggerExpress.register(_this._app);

        var _port = 10010;
        if(config && 'port' in config) {
            _port = config.port;
        }

        if(config && 'model' in config) {
            chatbot.setModel(config.model)
            .fail(function(err) {
                console.error('Error:', err);
            })
        }
        _this._server = _this._app.listen(_port, ready);

        //console.log('To watch the doc, visit http://127.0.0.1:' + port + '/docs');
    });

    _this.close = function() {
        if(_this._server) {
            _this._server.close();
        }
        else {
            console.error('Server not set');
        }
    }

    return _this;
}
