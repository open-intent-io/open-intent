#!/bin/bash

pushd /src/bindings/nodejs/modules/open-intent/
npm link
popd

/src/bindings/nodejs/modules/open-intent/bin/open-intent.js project create --no-npm chatbot

cd chatbot/
npm link open-intent
cp /custom_res/* res/

rm -r /usr/src/chatbot/test
mv /test /usr/src/chatbot/
npm install should
mocha


