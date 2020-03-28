#!/bin/bash
echo Test 2
prog=$(find "$(pwd)" -name driver2.out)
wc=$($prog | sed 's/,,/, ,/g;s/,,/, ,/g' | column -t -s, -n | wc -l)
wc=$((wc - 1))
if [ $wc -eq 1000 ] 
then
    echo "Passed with $wc records"
else
    echo "Failed, printed $wc records "
fi