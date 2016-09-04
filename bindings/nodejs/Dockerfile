FROM ubuntu:xenial
MAINTAINER open-intent.io

#nodejs
RUN apt-get update; apt-get install -y nodejs npm

ADD bin /open-intent/bin
ADD config /open-intent/config
ADD lib /open-intent/lib
ADD project-skeletons /open-intent/project-skeletons
ADD package.json /open-intent/package.json
ADD index.js /open-intent/index.js

RUN ln -s /usr/bin/nodejs /usr/bin/node
RUN cd /open-intent && \
    npm link && \
    cd / && \
    npm link open-intent

EXPOSE 5000
EXPOSE 5001
EXPOSE 5002

VOLUME /chatbot
WORKDIR /chatbot

ADD docker/entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]