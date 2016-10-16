#!/bin/bash

PACKAGE_TARBALL=/tmp/open-intent.tgz

if [ ! -n "$(ls -A /chatbot)" ]
then
    echo "New chatbot detected, initializing resources..."

    cd /tmp

    echo "Creating chatbot project"
    open-intent project create --no-npm chatbot > /dev/null 2>&1

    cd chatbot

    echo "Install the packages"
    npm install ${PACKAGE_TARBALL}
    npm install

    echo "Move chatbot project to /chatbot"
    mv /tmp/chatbot/* /chatbot

    echo "Chatbot project is ready"
else
    echo "Existing chatbot detected."
fi
