FROM open-intent
MAINTAINER open-intent.io

WORKDIR /

RUN npm install -g mocha

RUN open-intent project create --no-npm chatbot-test

WORKDIR /chatbot-test

RUN npm link open-intent && npm install

CMD ["mocha"]