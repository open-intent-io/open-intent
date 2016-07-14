#!/usr/bin/env node

var app = require('commander');

app
    .command('project <action>', 'project actions');

app.parse(process.argv);