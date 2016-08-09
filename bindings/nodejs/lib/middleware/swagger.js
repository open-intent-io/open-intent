
var express = require('express');

function MiddlewareInterface(port) {
    var _this = this;

    this._app = express();
    this._port = (port) ? port : 8080;

    this.attach = function(chatbot) {
        var swaggerConfig = {
            appRoot: __dirname
        };

        process.env.SUPPRESS_NO_CONFIG_WARNING = 'true';
        var SwaggerExpress = require('swagger-express-mw');
        var SwaggerUi = require('swagger-tools/middleware/swagger-ui');
        var express = require('express');

        _this._server = undefined;
        _this._app.set('chatbot', chatbot);

        SwaggerExpress.create(swaggerConfig, function(err, swaggerExpress) {
            if (err) { throw err; }

            // Add swagger-ui (This must be before swaggerExpress.register)
            _this._app.use(SwaggerUi(swaggerExpress.runner.swagger));

            // install middleware
            swaggerExpress.register(_this._app);

            _this._server = _this._app.listen(_this._port, ready);

            //console.log('To watch the doc, visit http://127.0.0.1:' + port + '/docs');
        });
    };

    this.detach = function() {
        if(_this._server) {
            _this._server.close();
        }
        else {
            console.error('Server not set');
        }
    }

    return this;
}

module.exports = function(port) {
    return new MiddlewareInterface(port);
};