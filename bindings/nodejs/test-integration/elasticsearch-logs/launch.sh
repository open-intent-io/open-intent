#!/bin/bash

set -o pipefail

test_directory=elasticsearch-logs

image_name=open-intent-${test_directory}-test

docker build -t ${image_name} ${test_directory}

docker run -d -i -p 9200:9200 --name elasticsearch elasticsearch

echo "Sleep 7 seconds during elasticsearch initialization to be sure everything is up and running."
sleep 7

# Run the test
docker run --rm -i --volumes-from open-intent-source-volume --link elasticsearch:elasticsearch ${image_name}

docker stop elasticsearch
docker rm elasticsearch