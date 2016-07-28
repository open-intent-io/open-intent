#!/bin/bash

docker create --name open-intent-source-volume -v $(pwd):/src ubuntu:xenial /bin/false

docker run --rm --volumes-from open-intent-source-volume -v /var/run/docker.sock:/var/run/docker.sock -v $(which docker):/usr/bin/docker open-intent-build /src/scripts/build_and_test.sh
