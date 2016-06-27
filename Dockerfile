FROM ubuntu:xenial

RUN apt-get update && \
    apt-get install -y libboost-system1.58-dev \
        libboost-regex1.58-dev \
        libboost-log1.58-dev \
        cmake git g++ \
        doxygen node-gyp

ADD . /src/

WORKDIR /build

RUN cd /src && git submodule update --init --recursive 
RUN cd /build && cmake -G "Unix Makefiles" /src

