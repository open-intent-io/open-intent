
var proxyquire = require('proxyquire');

var helpers = function() {
    return require('../../../project-skeletons/rest-chatbot/test/helpers')('project-skeletons/rest-chatbot/res');
}

proxyquire('../../../project-skeletons/rest-chatbot/test/test-chatbot', {
    'open-intent': require('../../../index'),
    './helpers': helpers
});