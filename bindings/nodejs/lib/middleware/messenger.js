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

var bodyParser = require("body-parser")
var messengerConfig = require('../config/messenger/default.json');

var messengerJsonParser = bodyParser.json({type:"application/json", verify: verifyRequestSignature });

const FACEBOOK_APP_SECRET = process.env.MESSENGER_APP_SECRET || 
                   messengerConfig.MESSENGER_APP_SECRET;

// Arbitrary value used to validate a webhook
const FACEBOOK_VALIDATION_TOKEN = process.env.MESSENGER_VALIDATION_TOKEN ||
                         messengerConfig.MESSENGER_VALIDATION_TOKEN;

// Generate a page access token for your page from the App Dashboard
const FACEBOOK_PAGE_ACCESS_TOKEN = process.env.MESSENGER_PAGE_ACCESS_TOKEN ||
                          messengerConfig.MESSENGER_PAGE_ACCESS_TOKEN;

if (!(FACEBOOK_APP_SECRET && FACEBOOK_VALIDATION_TOKEN && FACEBOOK_PAGE_ACCESS_TOKEN)) {
  console.error("Missing config values");
  process.exit(1);
}

function receivedMessage(chatbotClient, event) {
  var senderID = event.sender.id;
  var recipientID = event.recipient.id;
  var timeOfMessage = event.timestamp;
  var message = event.message;

  console.log("Received message for user %d and page %d at %d with message:", 
    senderID, recipientID, timeOfMessage);
  console.log(JSON.stringify(message));

  var isEcho = message.is_echo;
  var messageId = message.mid;
  var appId = message.app_id;
  var metadata = message.metadata;

  // You may get a text or attachment but not both
  var messageText = message.text;
  var messageAttachments = message.attachments;
  var quickReply = message.quick_reply;

  if (isEcho) {
    // Just logging message echoes to console
    console.log("Received echo for message %s and app %d with metadata %s", 
      messageId, appId, metadata);
    return;
  } else if (quickReply) {
    var quickReplyPayload = quickReply.payload;
    console.log("Quick reply for message %s with payload %s",
      messageId, quickReplyPayload);

    sendTextMessage(senderID, "Quick reply tapped");
    return;
  }

  if (messageText) {
    chatbotClient.talk(senderID, messageText).then(function(replies) {
      var reply = replies.length ? replies[0] : "An error occured";
      console.log("[messenger] Sending reply : "+reply);
      sendTextMessage(senderID, reply);
    });
  } else if (messageAttachments) {
    sendTextMessage(senderID, "Message with attachment received");
  }
}

/*
 * Send a text message using the Send API.
 *
 */
function sendTextMessage(recipientId, messageText) {
  var messageData = {
    recipient: {
      id: recipientId
    },
    message: {
      text: messageText,
      metadata: "DEVELOPER_DEFINED_METADATA"
    }
  };

  callSendAPI(messageData);
}


/*
 * Call the Send API. The message data goes in the body. If successful, we'll 
 * get the message id in a response 
 *
 */
function callSendAPI(messageData) {
  request({
    uri: 'https://graph.facebook.com/v2.6/me/messages',
    qs: { access_token: FACEBOOK_PAGE_ACCESS_TOKEN },
    method: 'POST',
    json: messageData

  }, function (error, response, body) {
    if (!error && response.statusCode == 200) {
      var recipientId = body.recipient_id;
      var messageId = body.message_id;

      if (messageId) {
        console.log("Successfully sent message with id %s to recipient %s", 
          messageId, recipientId);
      } else {
      console.log("Successfully called Send API for recipient %s", 
        recipientId);
      }
    } else {
      var errorMessage = response.error.message;
      var errorCode = response.error.code;
      console.error("Unable to send message. Error %d: %s", 
        errorCode, errorMessage);
    }
  });  
}

function verifyRequestSignature(req, res, buf) {
  var signature = req.headers["x-hub-signature"];

  if (!signature) {
    // For testing, let's log an error. In production, you should throw an 
    // error.
    console.error("Couldn't validate the signature.");
  } else {
    var elements = signature.split('=');
    var method = elements[0];
    var signatureHash = elements[1];

    var expectedHash = crypto.createHmac('sha1', FACEBOOK_APP_SECRET)
                        .update(buf)
                        .digest('hex');

    if (signatureHash != expectedHash) {
      throw new Error("Couldn't validate the request signature.");
    }
  }
}

/**
 * Super ugly workaround because facebook doesn't send the type:'application/json' header
 */
function attachContentTypeHeader(req, res, next) {
    req.headers["content-type"] = "application/json";
    console.log("req.body : "+req.body.toString());
    next();
}

module.exports.attach = function(chatbotClient, app) {
    /*
    * Use your own validation token. Check that the token used in the Webhook 
    * setup is the same token used here.
    *
    */
    app.use(messengerJsonParser);

    app.get('/messenger/chat', attachContentTypeHeader, messengerJsonParser, function(req, res) {
        console.log("headers : "+JSON.stringify(req.headers));
        console.log("header : "+JSON.stringify(req.header));
        if (req.query['hub.mode'] === 'subscribe' &&
            req.query['hub.verify_token'] === FACEBOOK_VALIDATION_TOKEN) {
            console.log("Validating webhook");
            res.status(200).send(req.query['hub.challenge']);
        } else {
            console.log("Request headers : "+JSON.stringify(req.headers))
            console.log("Request body : "+JSON.stringify(req.body))
            console.log("Current validation token : "+FACEBOOK_VALIDATION_TOKEN)
            console.error("Failed validation. Make sure the validation tokens match.");
            res.sendStatus(403);          
        }  
    });

    /*
    * All callbacks for Messenger are POST-ed. They will be sent to the same
    * webhook. Be sure to subscribe your app to your page to receive callbacks
    * for your page. 
    * https://developers.facebook.com/docs/messenger-platform/product-overview/setup#subscribe_app
    *
    */
    app.post('/messenger/chat', attachContentTypeHeader, messengerJsonParser, function (req, res) {
    var data = req.body;

    if (data.object == 'page') {
        data.entry.forEach(function(pageEntry) {
            var pageID = pageEntry.id;
            var timeOfEvent = pageEntry.time;

            pageEntry.messaging.forEach(function(messagingEvent) {
                if (messagingEvent.message) {
                    receivedMessage(chatbotClient, messagingEvent);
                } else {
                    console.log("Webhook received unknown messagingEvent: ", messagingEvent);
                }
            });
        });

        res.sendStatus(200);
    }
    else
    {
        res.sendStatus(404);
    }
    });
};
