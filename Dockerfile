FROM ubuntu:xenial

RUN apt-get update && \
    apt-get install -y libboost-system1.58-dev \
        libboost-regex1.58-dev \
        libboost-log1.58-dev \
        cmake g++ \
        doxygen node-gyp nodejs npm

ADD . /src/

WORKDIR /build

RUN ln -s /usr/bin/nodejs /usr/bin/node
RUN npm install -g swagger 
RUN cd /build && cmake -G "Unix Makefiles" /src

