FROM ubuntu:xenial
MAINTAINER open-intent.io

ENV LD_LIBRARY_PATH="/usr/local/lib:/usr/lib/x86_64-linux-gnu"

#nodejs
RUN apt-get update; apt-get install -y nodejs npm \
        libboost-system1.58 \
        libboost-regex1.58 \
        libboost-log1.58

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
    open-intent project create chatbot

WORKDIR /chatbot

VOLUME /app

EXPOSE 5000
EXPOSE 5001
EXPOSE 5002

ADD docker/entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]