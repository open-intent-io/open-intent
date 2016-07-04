#!/bin/bash

license_file='./LICENSE'

directories="
src/ 
include/ 
bindings/nodejs/src 
bindings/nodejs/include
bindings/nodejs/examples
bindings/nodejs/modules
test/tests
test/mock
"

no_pattern="node_modules"

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

no=`echo $f | grep "${no_pattern}"`

if [ "$no" != "" ]
then
   #echo "$f skipped"
   continue 
fi

echo "Processed file $f"
remove_first_comment=`cat $f | sed -r ':a;$!{N;ba};s|/\*[^*]*\*+([^/*][^*]*\*+)*/||'`
filtered=`echo "$remove_first_comment" | sed ':a;N;$!ba;s/^\s*\([^\s]\)/\1/g'`

#mv $f $f.old
echo "${header}${filtered}" > $f
echo "License Header copied to $f"
done   

