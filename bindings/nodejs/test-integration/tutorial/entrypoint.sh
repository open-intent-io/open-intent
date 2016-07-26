#!/bin/bash

pushd /src/
npm link
popd

/src/bindings/nodejs/bin/open-intent.js project create --no-npm chatbot

cd chatbot/
npm link open-intent
cp /custom_res/* res/

rm -r /usr/src/chatbot/test
mv /test /usr/src/chatbot/
npm install should
mocha


