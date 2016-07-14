
var objectPath = require("object-path");

module.exports = Model;

function safeGet(object, path) {
    return objectPath.get(object, path);
}

function Model() {
    this._data = {
    };
    
    this.oiml = function() {
        return safeGet(this._data, 'oiml');
    };
    
    this.commands = function() {
        return safeGet(this._data, 'commands.script');
    };

    this.dictionary = function() {
        return safeGet(this._data, 'dictionary');
    };

    this.hasJsCommands = function() {
        return safeGet(this._data, 'commands.type') == 'js';
    };

    this.hasRestCommands = function() {
        return safeGet(this._data, 'commands.type') == 'REST'
    };

    this.setOiml = function(oiml) {
        this._data.oiml = oiml;
    }

    this.setCommands = function(type, script) {
        this._data.commands = {};
        this._data.commands.type = type;
        this._data.commands.script = script;
    }

    this.setDictionary = function(dictionary) {
        this._data.dictionary = dictionary;
    }
}