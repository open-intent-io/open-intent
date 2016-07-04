FROM ubuntu:xenial
MAINTAINER open-intent.io

ENV LD_LIBRARY_PATH="/usr/local/lib:/usr/lib/x86_64-linux-gnu"

#nodejs
RUN apt-get update; apt-get install -y nodejs npm \
        libboost-system1.58 \
        libboost-regex1.58 \
        libboost-log1.58

RUN npm install -g nodemon

RUN ln -s /usr/bin/nodejs /usr/bin/node

COPY app.js /usr/src/
COPY terminal-client.js /usr/src/
COPY node_modules/ /usr/src/node_modules/
COPY package.json /usr/src/


#position the workspace in an example
WORKDIR /usr/src

ENV SUPPRESS_NO_CONFIG_WARNING=y
#VOLUME /usr/src/res

# Lock outgoing packets for security reason
CMD ["nodemon", "-w", "/usr/src/*", "-w", "/usr/src/res/*", "-e", "js,txt,json", "app.js"]
