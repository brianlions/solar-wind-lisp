#!/bin/bash
#
# unitTestRun.sh
#
#  Created on: Aug 17, 2011
#  Author:     Brian Yi ZHANG
#  Email:      brianlions@gmail.com
#

cmd=''
if [ $# -gt 0 ]; then
    if [ x"$1" == x"--valgrind" -o x"$1" == x"--vg" ]; then
        cmd='valgrind -q --tool=memcheck --leak-check=full '
        shift
    fi
fi

if [ $# -gt 0 ]; then
    failedTests=""
    testIdx=0
    for f in $@; do
        testIdx=`expr $testIdx + 1`
        echo -ne "\t@@@@@ running test $testIdx / $# : \`\`$f'' @@@\n";
        $cmd$f;
        if [ $? -ne 0 ]; then
            failedTests="$failedTests\n\t$f"
        fi
        echo -ne "\t----- finished test $testIdx / $# : \`\`$f'' -----\n\n\n";
    done

    if [ "$failedTests" != "" ]; then
        echo -e "\t\033[1;31m@@@@@ FOLLOWING TESTS FAILED @@@@@\n$failedTests\n\033[m"
    else
        echo -e "\t\033[1;32m@@@@@ all $# tests passed @@@@@\033[m"
    fi
fi
