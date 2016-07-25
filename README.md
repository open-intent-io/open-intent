<p align="center">
  <img src="/doc/img/logo.png" alt="Logo"/>
</p>

====================
[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)][MIT License] [![Gitter](https://badges.gitter.im/open-intent-io/open-intent.svg)](https://gitter.im/open-intent-io/open-intent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![Build Status](https://travis-ci.org/open-intent-io/open-intent.svg?branch=master)](https://travis-ci.org/open-intent-io/open-intent)

open-intent is a natural language processing framework with the aim of finding user intents in sentences based on entity matching.
The framework allows you to build and deploy very easily your conversational bot in minutes.

*Supported operating systems:*

[![Linux tested](https://img.shields.io/badge/Linux-tested-brightgreen.svg)]() [![Windows not tested](https://img.shields.io/badge/Windows-not%20tested-orange.svg)]() [![OS X not tested](https://img.shields.io/badge/OS%20X-not%20tested-orange.svg)]()

# Example

This is as simple as those 3 lines to start prototyping your bot.

    npm install -g open-intent
    open-intent project create mychatbot
    open-intent project start mychatbot

This will create a JS project in mychatbot/ directory with the necessary files so that you can start editing the model files located in res/ directory.

Once started, an IRC client is waiting for you to type your first message. Say 'hello' to your newly created bot.

We have created a [tutorial](https://github.com/open-intent-io/open-intent/wiki/Time-bot-tutorial) for you to quickly learn how to create your first bot with open-intent.

# Documentation

The documentation can be found [here](https://github.com/open-intent-io/open-intent/wiki) and will help you learn how to use every feature of the open-intent framework.

# Source

Our latest source of open-intent can be found on [GitHub]. Fork us!


# Contributing to open-intent

Follow [contributing](CONTRIBUTING.md) file.

## Build

There are two ways to build the framework. You can do it with CMake or Docker.
Note that for the moment the framework has only been tested on Ubuntu 16.04.

### CMake
To build, configure with CMake and build.

    mkdir build-dir/
    cd build-dir/
    cmake -G "Unix Makefiles" ..
    make -j 4

### Docker
To build, just type:

    docker build -t open-intent-build .
    docker run open-intent-build build.sh

That's it!

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