FROM ubuntu:xenial

RUN apt-get update && \
    apt-get install -y libboost-system1.58-dev \
        libboost-regex1.58-dev \
        libboost-log1.58-dev \
        cmake g++ git \
        doxygen node-gyp nodejs npm clang-format-3.7

VOLUME /src
WORKDIR /src

RUN ln -s /usr/bin/nodejs /usr/bin/node


