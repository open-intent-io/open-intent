#!/bin/bash

set -o pipefail

test_directory=chatbot

image_name=open-intent-${test_directory}-test

docker build -t ${image_name} ${test_directory}

# Run the test
docker run --rm -i --volumes-from open-intent-source-volume ${image_name}
