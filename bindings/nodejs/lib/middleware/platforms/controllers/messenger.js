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

module.exports.attach = MessengerBot;

var Bot = require('messenger-bot');

function MessengerBot(chatbotClient, messengerConfig, app) {

    const FACEBOOK_APP_SECRET = process.env.MESSENGER_APP_SECRET ||
        messengerConfig.MESSENGER_APP_SECRET;

     // Arbitrary value used to validate a webhook
    const FACEBOOK_VALIDATION_TOKEN = process.env.MESSENGER_VALIDATION_TOKEN ||
        messengerConfig.MESSENGER_VALIDATION_TOKEN;

    // Generate a page access token for your page from the App Dashboard
    const FACEBOOK_PAGE_ACCESS_TOKEN = process.env.MESSENGER_PAGE_ACCESS_TOKEN ||
        messengerConfig.MESSENGER_PAGE_ACCESS_TOKEN;

    const SEND_API_URI = process.env.SEND_API_URI || 'https://graph.facebook.com/v2.6/me/messages';

    if (!FACEBOOK_APP_SECRET) {
        throw new Error("FACEBOOK_APP_SECRET is missing");
    }

    if(!FACEBOOK_VALIDATION_TOKEN) {
        throw new Error("FACEBOOK_VALIDATION_TOKEN is missing");
    }

    if(!FACEBOOK_PAGE_ACCESS_TOKEN) {
        throw new Error("FACEBOOK_PAGE_ACCESS_TOKEN is missing");
    }

    var bot = new Bot({
        token: FACEBOOK_PAGE_ACCESS_TOKEN,
        verify: FACEBOOK_VALIDATION_TOKEN,
        app_secret: FACEBOOK_APP_SECRET 
    });

    app.use(bot.middleware());


    bot.on('message', function(payload, reply) {
        receivedMessage(chatbotClient, payload, reply);
    });

    bot.on('postback', function(payload, reply) {
        receivedPostback(chatbotClient, payload, reply);
    });

    function JsonParseWithExceptionHandling(str) {
        var data = undefined;
        try {
            data = JSON.parse(str);
        } catch (e) {
            return;
        }
        return data;
    }

    function extractAttachment(message) {
        var myRegexp = /^attachment\((.*?)\)$/g;
        var match = myRegexp.exec(message);
        var attachment = undefined;
        if(match && match.length > 0) {
            attachment = JsonParseWithExceptionHandling(match[1]);
        }
        return attachment;
    }

    function receivedPostback(chatbotClient, event, reply) {
        //console.log(event);
        var senderID = event.sender.id;
        var postback = event.postback;
        if(postback) {
            var message = postback.payload;
            chatbotClient.talk(senderID, message).then(function(replies) {
                var reply_data = replies.length ? replies[0] : "Error while getting reply.";
                reply({ text: reply_data }, function(err, res) {
                    if(err) {
                        console.error(err);
                        return;
                    }
                });
            });
        }
    }

    function receivedMessage(chatbotClient, event, reply) {
        //console.log(event);
        var senderID = event.sender.id;
        var message = event.message;
        var isEcho = message.is_echo;

        // You may get a text or attachment but not both
        var messageText = message.text;
        var messageAttachments = message.attachments;
        var quickReply = message.quick_reply;

        if (isEcho) {
            return;
        } else if (quickReply) {
            return;
        }

        if (messageText) {
            chatbotClient.talk(senderID, messageText).then(function(replies) {
                var reply_data = replies.length ? replies[0] : "Error while getting reply.";
                var attachment = extractAttachment(reply_data);
                
                if(attachment) { 
                    reply(attachment);
                }
                else {
                    reply({ text: reply_data });
                }
            });
        } else if (messageAttachments) {
            reply({ text: "I do not handle attachments yet..." });
        }
    }
    

}

