#!/bin/bash

license_file='./LICENSE'

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

f=$1

echo "File selected $f"

if [ ! -f "$f" ]; then
    echo "File not found!"
    exit 1
fi

remove_first_comment=`cat $f | sed -r ':a;$!{N;ba};s|/\*[^*]*\*+([^/*][^*]*\*+)*/||'`
filtered=`echo "$remove_first_comment" | sed ':a;N;$!ba;s/^\s*\([^\s]\)/\1/g'`

echo "${header}${filtered}" | tee  test.file
