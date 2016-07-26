#!/bin/bash

test_directory=$1

image_name=open-intent-${test_directory}-test

docker build -t ${image_name} ${test_directory}

# Run the test
docker run --rm --volumes-from open-intent-source-volume ${image_name}
