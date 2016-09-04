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
var path = require('path');
var fs = require('fs');

var helpers = require('./helpers')();

var testConversationDirectory = path.resolve(__dirname, '..', '..', 'test-conversations');
var files = fs.readdirSync(testConversationDirectory);
var filenames = [];

for(var j in files) {
    if(path.extname(files[j]) === ".txt") {
        filenames.push(files[j]);
    }
}

function extractScript(filename) {
    var filepath = path.resolve(testConversationDirectory, filename);
    var content = fs.readFileSync(filepath, 'utf-8');
    return content.split(/\r?\n/);
}



describe('Given a script, the chatbot', function() {
    beforeEach(function(done) {
        helpers.beforeEach(done);
    });

    afterEach(function(done) {
        helpers.afterEach(done);
    });

    for(i in filenames) {
        (function() {
            var filename = filenames[i];
            it('should follow the conversation in "' + filename + '"', function(done) {
                var script = extractScript(filename);
                helpers.checkScript(script, done);
            });
        })();
    }
});
