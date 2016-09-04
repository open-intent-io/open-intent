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
var should = require('should');

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

describe('Food bot tests', function() {
   it('should ask what the customer wish and she is supposed to answer salad', function(done) {
      var expected_output = [
         "> Would you want to eat a pizza, a hamburger or a salad?",
         "> Got it, you want salad, right?",
         "> I'm ordering, it is gonna be 5$.",
         ">"
      ];

      exec('echo "Hello\nSalad\nyes\nquit\n" | nodejs app.js', function(error, stdout, stderr) {
         if(error) {
            console.error(error);
            return;
         }

         stdout = stdout.split('\n');
         compare(expected_output, stdout);
         done();
      });
   });
});
