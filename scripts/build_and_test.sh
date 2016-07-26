#!/bin/bash

BUILD_DIR=build-dir

# Apply license and format to check if it has been applied correctly before pushing the branch

echo "Check format and license"
./scripts/apply_format_and_license.sh
MODIFIED_FILES_COUNT=`git diff --name-only | wc -l`

if [ "$MODIFIED_FILES_COUNT" -gt "0" ]
then
    MODIFIED_FILES=`git diff --name-only`
    echo "Format or license has not been applied to the following files:"
    echo -e "${MODIFIED_FILES}"
    exit 1
fi



mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

echo "Configuring..."
cmake -G "Unix Makefiles" ..

echo "Building..."
make -j 4

make run-all-tests


