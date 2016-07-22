#!/bin/bash

pushd /src/bindings/nodejs/modules/open-intent/
npm link
popd

echo "Create the chatbot project"
/src/bindings/nodejs/modules/open-intent/bin/open-intent.js project create --no-npm chatbot

cd chatbot/
npm link open-intent

expected_output=">>> Ready to talk to the chatbot <<<
> Would you want to eat a pizza, a hamburger or a salad?
> Got it, you want salad, right?
> I'm ordering, it is gonna be 5$.
> "

echo "Execute the chatbot"
actual_output=`echo -e "Hello\nSalad\nyes\nquit\n" | nodejs app.js`

if [ "$actual_output" = "$expected_output" ]
then
    echo "Success"
    exit 0
else
    echo "Failure"
    exit 1
fi
