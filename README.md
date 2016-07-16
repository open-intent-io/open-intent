<p align="center">
  <img src="/doc/img/logo.png" alt="Logo"/>
</p>

====================
[![Build Status](https://travis-ci.org/open-intent-io/open-intent.svg?branch=master)](https://travis-ci.org/open-intent-io/open-intent)

open-intent is a natural language processing framework with the aim of finding user intents in sentences based on entity matching.
The framework allows you to build and deploy easily your conversational bot in minutes.

# Example

This is as simple as those 3 lines to start prototyping your bot.

    npm install -g open-intent
    open-intent project create chatbot
    open-intent project start chatbot

This will create a directory chatbot/ with necessary files so that you can start editing the model which is in res/ directory.

Please follow this [link](/bindings/nodejs/modules/open-intent) for a more detailed documentation


# Source
Our latest source of open-intent can be found on [GitHub]. Fork us!

# Contributing to open-intent
Follow [contributing](CONTRIBUTING.md) file.
## Build

There are two ways to build the framework. You can do it with CMake or Docker.

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