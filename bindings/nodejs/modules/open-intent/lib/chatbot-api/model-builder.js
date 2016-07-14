
var fs = require('fs');
var checkModel = require('./model-checker');
var Model = require('./model');

module.exports = ModelBuilder;

function ModelBuilder() {

    this._model = new Model();

    this.withDictionaryFromFile = function(filename) {
        this._model.setDictionary(fs.readFileSync(filename, 'utf-8'));
        return this;
    };

    this.withOIMLFromFile = function(filename) {
        this._model.setOiml(fs.readFileSync(filename, 'utf-8'));
        return this;
    };

    this.withJsUserCommandsFromFile = function(filename) {
        this._model.setCommands('js', fs.readFileSync(filename, 'utf-8'));
        return this;
    };

    this.withRestUserCommandsFromFile = function(filename) {
        this._model.setCommands('REST', fs.readFileSync(filename, 'utf-8'));
        return this;
    };

    this.build = function(fn) {

        if(fn) {
            var err = checkModel(this._model);

            if(err) {
                fn(err);
            }
            else {
                fn(undefined, this._model);
            }
        }

        if(err) {
            return undefined;
        }
        else {
            return this._model;
        }
    }

    return this;
}
