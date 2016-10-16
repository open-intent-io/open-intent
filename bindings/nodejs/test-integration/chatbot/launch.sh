#!/bin/bash

set -o pipefail

test_directory=chatbot

image_name=open-intent-${test_directory}-test

echo "Build docker image ${image_name}"
docker build -t ${image_name} ${test_directory}

echo "Run docker image ${image_name}"
# Run the test
docker run --rm -i --volumes-from open-intent-source-volume ${image_name}
