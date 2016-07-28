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
'use strict';

const 
  crypto = require('crypto'),
  request = require('request'),
  express = require('express'),
  fs = require('fs'),
  ChatbotClient = require('open-intent/lib/rest-client')

var app = express();
var chatbot_client = new ChatbotClient("http://localhost:8080");

app.set('port', process.env.PORT || 5000);
app.use(express.static('public'));

var baseUrl = require("./config/general").baseUrl;

var platforms = {};
platforms['kik'] = require("./controllers/kik");
platforms['messenger'] = require("./controllers/messenger");
platforms['skype'] = require("./controllers/skype");
platforms['slack'] = require("./controllers/slack");
platforms['telegram'] = require("./controllers/telegram");

var selection = require("./config/selection")

function deployPlatform(platformName, platform, isSelected) {  
   var message = platformName;
   var webhook = baseUrl;
   webhook += platformName === 'kik' ? '/incoming' : '/'+platformName+'/chat';
   if (selection[platformName]) {
     console.log("setting up "+platformName);
     if (platformName !== 'telegram' &&  platformName !== 'slack')
        console.log("the webhook is : "+webhook);  
     platform.attach(chatbot_client, app);   
   } else {
   }
}

for (var platformName in platforms) {
    if (platforms.hasOwnProperty(platformName)) {
      deployPlatform(platformName, platforms[platformName], app);  
    }
}

console.log("Server listening at "+app.get('port'));
app.listen(app.get('port')); 
