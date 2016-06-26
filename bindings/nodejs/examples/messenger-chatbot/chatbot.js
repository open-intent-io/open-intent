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
var OpenIntent = require('../../build/Release/open-intent');
var fs = require('fs');

var dictionaryFile = './dictionary.json';
var openIntentMLFile = './script.oiml';

var dictionaryModel = fs.readFileSync(dictionaryFile);
var interpreterModel = fs.readFileSync(openIntentMLFile);

menu = {
    "Coca-Cola": 4.5,
    "Kronenbourg": 5.5,
    "Heineken": 5.7
}

function computeDescription(order) {
    var str = "";
    for(var i in order) {
        str += '' + order[i] + 'x ' + i + ', ';
    }

    return str.substring(0, str.length - 2);
}

function appendToOrder(order, quantity, beverage) {
    if(!order[beverage]) {
        order[beverage] = 0;
    }
    order[beverage] += parseInt(quantity);
}

function computePrice(order) {
    var price = 0.0;
    for(var i in order) {
        itemPrice = menu[i];

        if(itemPrice) {
            price += parseFloat(order[i] * itemPrice);
        }
    }

    return price;
}

module.exports = function() {

    this._clients = {};

    this._replyCallback = function(sessionId, message)
    {
        this._userDefinedCallback(sessionId, message);
    }

    this._handleActionCallback = function(sessionId, action, intentVariables, userDefinedVariables)
    {
        console.log("intentVariables: " + JSON.stringify(intentVariables));
        console.log("userDefinedVariables: " + JSON.stringify(userDefinedVariables));

        if(userDefinedVariables["order_description"] == undefined) {
            userDefinedVariables["order_description"] = "";
        }

        if(action == '#append_to_order') {
            if("@number0" in intentVariables && "@beverage0" in intentVariables) {
                appendToOrder(this._clients[this._currentSession], intentVariables["@number0"], intentVariables["@beverage0"]);
                userDefinedVariables["order_description"] = computeDescription(this._clients[this._currentSession]);
            }

            if("@number1" in intentVariables && "@beverage1" in intentVariables) {
                appendToOrder(this._clients[this._currentSession], intentVariables["@number1"], intentVariables["@beverage1"]);
                userDefinedVariables["order_description"] = computeDescription(this._clients[this._currentSession]);
            }
        }
        else if(action == "#order") {
            var price = computePrice(this._clients[this._currentSession]);

            userDefinedVariables['price'] = '' + price;
        }
        else if(action == "#clean_order") {
            this._clients[this._currentSession] = {};

            for(var i in userDefinedVariables) {
                delete userDefinedVariables[i];
            }
        }
    }

    this._chatbot = OpenIntent.createMultiSessionChatbotFromOIML(dictionaryModel, interpreterModel,
        this._replyCallback, this._handleActionCallback);
    this._userDefinedCallback;

    this.treatMessage = function(sessionId, message, fn) {

        if(!(sessionId in this._clients))
        {
            this._clients[sessionId] = {};
            chatbot.addSession(sessionId);
        }

        this._userDefinedCallback = fn;
        this._currentSession = sessionId;
        this._chatbot.treatMessage(sessionId, message);
        this._userDefinedCallback = undefined;
        this._currentSession = undefined;
    }

    this.addSession = function(sessionId) {
        this._chatbot.addSession(sessionId);
    }

    return this;
};
