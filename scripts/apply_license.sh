#!/bin/bash

license_file='./LICENSE'

directories="
src/ 
include/ 
bindings/nodejs/src 
bindings/nodejs/include
bindings/nodejs/lib
bindings/nodejs/test
test/tests
test/mock
"

no_pattern="node_modules
project-skeletons/chatbot/res/"

license=`cat ${license_file}`
header="/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

${license}
*/
"

# script to copy the headers to all the source files and header files
for f in $(find ${directories} -name '*.cpp' -or -name '*.hpp' -or -name '*.js' ); do

    no_process=0

    # Filter out some files
    while read -r line; do
        no=`echo $f | grep "${line}"`
        
        if [ "$no" != "" ]
        then
           no_process=1
        fi
    done <<< "$no_pattern";

    if [ "$no_process" -gt "0" ];
    then
        #echo "$f skipped"
        continue;
    fi

echo "Processed file $f"
file_content=`cat $f`

shebang=`echo "$file_content" | head -n 1 | grep "^#\!"`

filtered=`echo "$file_content"`
if [ ! -z "$shebang" ]
then
    echo "Shebang detected"
    shebang=`echo "${shebang}"`
    filtered=`echo "$file_content" | tail -n +2`
fi

filtered=`echo "$filtered" | sed -r ':a;$!{N;ba};s|/\*[^*]*\*+([^/*][^*]*\*+)*/||'`
filtered=`echo "$filtered" | sed ':a;N;$!ba;s/^\s*\([^\s]\)/\1/g'`

#mv $f $f.old
if [ ! -z "$shebang" ]
then
    (echo "${shebang}"; echo "${header}${filtered}") > $f
else
    echo "${header}${filtered}" > $f
fi
echo "License Header copied to $f"
done   

