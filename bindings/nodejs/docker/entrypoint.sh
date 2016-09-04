#!/bin/bash

if [ ! -n "$(ls -A /chatbot)" ]
then
    echo "New chatbot detected, initializing resources..."

    cd /tmp
    open-intent project create chatbot > /dev/null 2>&1
    mv /tmp/chatbot/* /chatbot

    cd /chatbot
else
    echo "Existing chatbot detected."
fi

open-intent project start chatbot