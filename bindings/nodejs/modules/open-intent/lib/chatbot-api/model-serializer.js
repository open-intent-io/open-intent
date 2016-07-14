
var Model = require('./model');

module.exports = {
    'serialize': serialize,
    'deserialize': deserialize
}

function serialize(deserializedModel) {
    return deserializedModel._data;
}

function deserialize(serializedModel) {
    var model = new Model();

    model.setOiml(serializedModel.oiml);
    model.setCommands(serializedModel.commands.type, serializedModel.commands.script);
    model.setDictionary(serializedModel.dictionary);

    return model;
}