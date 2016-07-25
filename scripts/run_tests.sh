#!/bin/bash

BUILD_DIR=build-dir

cd ${BUILD_DIR}

echo "Running tests"
make run-all-tests

echo "Running integration tests"
make run-nodejs-integration-tests