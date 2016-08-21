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

const skype = require('skype-sdk');

module.exports.attach = function(chatbotClient, skypeconfig, app) {

    const skypeBotId = process.env.SKYPE_BOT_ID || skypeconfig.BOT_ID;
    const skypeAppId = process.env.SKYPE_APP_ID || skypeconfig.APP_ID;
    const skypeAppSecret = process.env.SKYPE_APP_SECRET || skypeconfig.APP_SECRET;
    
    const botService = new skype.BotService({
        messaging: {
            botId: skypeBotId,
            serverUrl : "https://apis.skype.com",
            requestTimeout : 15000,
            appId: skypeAppId,
            appSecret: skypeAppSecret
        }
    });

    botService.on('contactAdded', (bot, data) => {
        bot.reply(`Hello ${data.fromDisplayName}!`, true);
    });


    botService.on('personalMessage', (bot, data) => {
        chatbotClient.talk(data.from, data.content).then(function(replies) {
            var reply = replies.length ? replies[0] : "An error occured";
            bot.reply(reply, true)
        });
    }); 

    app.post('/skype/chat', skype.messagingHandler(botService));
}
