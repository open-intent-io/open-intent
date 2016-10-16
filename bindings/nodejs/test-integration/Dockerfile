FROM open-intent
MAINTAINER open-intent.io

RUN npm install -g mocha

RUN /entrypoint.sh

WORKDIR /chatbot

CMD ["mocha"]