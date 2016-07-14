'use strict';

var path = require('path');

var config = {};

config.project = {
    skeletonsDir: path.resolve(__dirname, '..', 'project-skeletons')
};

config.debug = false;

module.exports = config