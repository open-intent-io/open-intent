#!/bin/bash

if [ ! -n "$(ls -A /app)" ]
then
    echo "New chatbot detected, initializing resources..."

    cp -r /chatbot/res /app/res
    cp -r /chatbot/config /app/config

    chmod ugo=rwx -R /app

else
    echo "Existing chatbot detected."
fi

rm -r /chatbot/res
rm -r /chatbot/config

ln -s /app/res /chatbot/res
ln -s /app/config /chatbot/config

open-intent project start chatbot