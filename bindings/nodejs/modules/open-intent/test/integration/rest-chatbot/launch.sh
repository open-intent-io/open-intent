#!/bin/bash

docker build -t open-intent-rest-chatbot-test .

docker run --rm --volumes-from source-volume open-intent-rest-chatbot-test
