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
var exec = require('child_process').exec;
var spawn = require('child_process').spawn;
var should = require('should');
var path = require('path');
var fs = require('fs');
var elasticsearch = require('elasticsearch');
var Q = require('q');

function call(fn) {
   return function() {
      return fn();
   }
}

function combine(arr1, arr2) {
   if(arr1.length == 0 && arr2.length == 0
       || arr1.length != arr2.length) {
      return [];
   }

   var val1 = arr1[0];
   var val2 = arr2[0];

   var tail1 = arr1.slice(1);
   var tail2 = arr2.slice(2);

   var output = [[val1, val2]];
   output.concat(combine(tail1, tail2));
   return output;
}

function compare(expected, actual) {
   should.equal(expected.length, actual.length);

   var expectedAndActual = combine(expected, actual);

   expectedAndActual.map(function(currentValue, index, arr) {
      var re = new RegExp(currentValue[0]);
      currentValue[1].should.match(re);
   });
}


describe('ElasticSearch logs tests', function() {
   var selectionFilename = path.resolve(__dirname, '..', 'config/loggers/selection.json');
   var elasticsearchFilename = path.resolve(__dirname, '..', 'config/loggers/elasticsearch.json');

   before(function() {
      var selection = require(selectionFilename);
      var elasticsearch = require(elasticsearchFilename);

      selection.elasticsearch = true;
      elasticsearch.host = 'elasticsearch';

      fs.writeFileSync(selectionFilename, JSON.stringify(selection, null, 4));
      fs.writeFileSync(elasticsearchFilename, JSON.stringify(elasticsearch, null, 4));

   });

   it('should have modified the elasticsearch selection config', function() {
      var selection = require(selectionFilename);
      should.equal(selection.elasticsearch, true);
   });

   it('should be able to ping the elasticsearch service', function(done) {
      var client = new elasticsearch.Client({
         host: 'elasticsearch:9200'
      });

      client.ping({
         // ping usually has a 3000ms timeout
         requestTimeout: Infinity,

         // undocumented params are appended to the query string
         hello: "elasticsearch!"
      }, function(err, res) {
         if (!err) {
            done();
         }
      });
   });

   describe('After launching the bot', function() {
      function executeChatbot() {
         this.timeout(5000);
         var defer = Q.defer();
         var app = spawn('nodejs', ['app.js'], {});

         app.on('exit', function() {
            defer.resolve();
         });

         /*app.stdout.on('data', function(data) {
            console.log('Chatbot stdout:', data.toString('utf-8'));
         });*/

         setTimeout(function() {
            //console.log('write stdin of application');
            app.stdin.write("Hello\nWhat is your name?\nAre you crazy?\n");
            setTimeout(function() {
               app.kill();
            }, 2000)
         }, 1500);

         return defer.promise;
      }


      before(executeChatbot);


      it('should have the right mapping', function(done) {
         function checkMapping(data) {
            data.should.have.property('conversations');
            data.conversations.should.have.property('mappings');
            data.conversations.mappings.should.have.property('logentry');
            data.conversations.mappings.logentry.should.have.property('properties');
         }

         function getMapping() {
            var client = new elasticsearch.Client({
               host: 'elasticsearch:9200'
            });

            return client.indices.getMapping({
                index: 'conversations',
                type: 'logentry'
             })
             .then(checkMapping);
         }

         getMapping().then(call(done));
      });

      it('should contain the conversation', function(done) {
         function checkConversationCount(response) {
            response.count.should.equal(6);
         }

         function getDocumentCount() {
            return client.count({
                index: 'conversations'
             })
             .then(checkConversationCount);
         }

         function extractMessage(item) {
            return item._source.data.message;
         }

         function checkConversation(response) {
            var hits = response.hits.hits;

            var expectedConversation = [
               "Hello",
               "Hello, I can tell you what time it is if you ask.",
               "What is your name?",
                "My name is Bob.",
                "Are you crazy?",
                "I don't understand what you mean..."
            ];
            var actualConversation = hits.map(extractMessage);

            compare(expectedConversation, actualConversation);
         }

         function getAllDocuments() {
            return client.search({
               index: 'conversations',
               body: {
                  query: {
                     match_all: {}
                  },
                  sort: { timestamp: { order: "asc" }}
               }
            }).then(checkConversation);
         }

         var client = new elasticsearch.Client({
            host: 'elasticsearch:9200'
         });

         getDocumentCount().then(getAllDocuments).then(call(done));
      });
   });
});
