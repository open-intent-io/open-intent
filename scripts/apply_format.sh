#!/bin/bash


CLANG='clang-format-3.7'
CLANG_STYLE=Google

directories="
src/ 
include/ 
"

# script to copy the headers to all the source files and header files
for f in $(find ${directories} -name '*.cpp' -or -name '*.hpp' ); do
echo "Processed file $f"

content=`${CLANG} -style=${CLANG_STYLE} $f`
echo "$content" > $f

done   

