#!/bin/bash
echo Test 1
prog=$(find "$(pwd)" -name driver1.out)
output=$($prog 2>&1 >/dev/null | sed 's/,,/, ,/g;s/,,/, ,/g' | column -t -s, -n) 
$output