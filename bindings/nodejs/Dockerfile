FROM ubuntu:xenial
MAINTAINER open-intent.io

#nodejs
RUN apt-get update; apt-get install -y nodejs npm
RUN ln -s /usr/bin/nodejs /usr/bin/node

COPY open-intent.tgz /tmp/open-intent.tgz
RUN npm install -g /tmp/open-intent.tgz

EXPOSE 5000
EXPOSE 5001
EXPOSE 5002

#VOLUME /chatbot
WORKDIR /chatbot

ADD docker/entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]