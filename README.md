<p align="center">
  <a href="https://www.open-intent.io">
    <img src="/doc/img/logo.png" alt="Logo"/>
  </a>
</p>

====================

[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)][MIT License] [![Gitter](https://badges.gitter.im/open-intent-io/open-intent.svg)](https://gitter.im/open-intent-io/open-intent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![Build Status](https://travis-ci.org/open-intent-io/open-intent.svg?branch=master)](https://travis-ci.org/open-intent-io/open-intent) ![platform](https://img.shields.io/badge/platform-linux%20only-green.svg)

[open-intent] is a framework that helps you create and deploy your bots in minutes thanks to [npm](https://preview.npmjs.com/package/open-intent) and [docker](https://hub.docker.com/r/openintent/chatbot/).
Open-intent will help you translate your user intents into actual business actions on messenger and your own website.

Please note that [open-intent] is only available on **Linux** for now.

# Example

This is as simple as those two lines to start prototyping your bot

    npm install -g open-intent
    open-intent project create mychatbot

This command starts an instance of bot with model located in the **mychatbot/res/** directory on your host. In
**mychatbot/config**, you'll find the configuration files used to plug your bot in the available platforms. For now the only fully tested
platform is **Messenger** but you can try the others at your own risks.

Once the configuration is done, you can test your bot right away with the following command:

    open-intent project start mychatbot
     
Say 'hello' to your new bot!  You can then edit the model or config files and the bot restarts automatically when files are saved.

You can also follow the [tutorial](https://github.com/open-intent-io/open-intent/wiki/Full-chatbot-deployment-tutorial) to write your first bot and deploy it on heroku for free.

## Your first bot model

3 files are required to describe a bot: the dictionary file, the model file and the user actions file. Below is the initial dictionary file you'll find in **mychatbot/res/** when you create the bot. It is used to teach your bot what words and concepts it'll have to deal with:

    {
      "entities": {
        "@greetings": {
          "hello": ["hi", "yep", "yo", "hey"]
        },
        "@food_type": {
          "pizza": [],
          "hamburger": ["big mac", "cheeseburger", "burger"],
          "salad": []
        },
        "@number": {
          "regex": "[0-9]+"
        },
        "@yes": {
          "yes": []
        },
        "@no": {
          "no": []
        }
      }
    }

The model file is a description of a typical conversation your bot will have to handle.
It is written with the Open-Intent Markup Language that is very close to natural language.
It uses the entities in the dictionary to detect the user intents (an ordered list of entities).
This is like teaching a little story that will drive the conversations of your user.

    {
    @root
        -Hello
            #greetings
        -Would you want to eat a pizza, a hamburger or a salad?
    @get_food_type
        -Pizza, please.
            #get_food_type
            *I did not understand. Pizza, hamburger or salad?
        -Got it, you want _, right?
    @yesno
        -Yes!
            #confirm
        -I'm ordering, it is gonna be _$.
    @end
    }

    {
    @yesno
        -No!
            #notunderstood
        -Tell me what you want. Pizza, hamburger or salad?
    @get_food_type
    }

Of course, there is a Javascript file to handle your business logic when actions (keyword prefixed with #) are triggered.

    var food_type;
    module.exports = {
        "#get_food_type": function(intentVariables, sessionId, next) {
            var replyVariables = {};
            food_type = intentVariables['food_type0'];
            replyVariables['0'] = food_type;
            next(replyVariables);
        },
        "#confirm": function(intentVariables, sessionId, next) {
            var replyVariables = {};
            if(food_type == 'pizza') {
                replyVariables['0'] = '8';
            }
            else {
                replyVariables['0'] = '5';
            }
            next(replyVariables);
        }
    }


This bot is a very simple one but there is a lot more in the open-intent framework. If you want to fully understand the model, we have created a
[tutorial](https://github.com/open-intent-io/open-intent/wiki/Time-bot-tutorial) to help you quickly
learn the basics on how to create your first bot with open-intent in 5 minutes.

# Documentation

The documentation can be found [here](https://github.com/open-intent-io/open-intent/wiki) and will help you learn how to use every feature of the open-intent framework.


# Contributing to open-intent

Follow [contributing](CONTRIBUTING.md) file.

License
---------------------

open-intent is **licensed** under the **[MIT License]**. The terms of the license are as follows:

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


[MIT License]: https://opensource.org/licenses/MIT
[GitHub]: https://github.com/open-intent-io/open-intent
[logo]: /doc/img/logo.png
[open-intent]: https://www.open-intent.io