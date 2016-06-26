#!/bin/bash

directories="
src/ 
include/ 
bindings/nodejs/src 
bindings/nodejs/include
bindings/nodejs/examples
test/tests
test/mock
"

# script to copy the headers to all the source files and header files
for f in $(find ${directories} -name '*.old' ); do
echo "Processed file $f"

rm -rf $f

done   

