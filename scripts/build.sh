#!/bin/bash

BUILD_DIR=build-dir

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

echo "Configuring..."
cmake -G "Unix Makefiles" .. 

echo "Building..."
make -j 4

echo "Running tests"
make run-all-tests
