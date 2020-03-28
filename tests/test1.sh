#!/bin/bash
echo Test 1
prog=$(find "$(pwd)" -name driver1.out)
wc=$($prog | sed 's/,,/, ,/g;s/,,/, ,/g' | column -t -s, -n | wc -l)
wc=$((wc - 1))
if [ $wc -eq 10 ] 
then
    echo "Passed with $wc records"
else
    echo "Failed, printed $wc records "
fi
