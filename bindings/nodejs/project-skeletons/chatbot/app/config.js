
var path = require('path');

var configDirectory = path.resolve(__dirname, '..', 'config');

module.exports = {
    'general': require(path.resolve(configDirectory, 'general.json')),
    'selection': require(path.resolve(configDirectory, 'selection.json')),
    'loggers': {
        'selection': require(path.resolve(configDirectory, 'loggers', 'selection.json')),
        'elasticsearch': require(path.resolve(configDirectory, 'loggers', 'elasticsearch.json'))
    },
    'kik': require(path.resolve(configDirectory, 'kik/default.json')),
    'messenger': require(path.resolve(configDirectory, 'messenger/default.json')),
    'skype': require(path.resolve(configDirectory, 'skype/default.json')),
    'slack': require(path.resolve(configDirectory, 'slack/default.json')),
    'telegram': require(path.resolve(configDirectory, 'telegram/default.json'))
};
