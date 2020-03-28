#!/bin/bash
echo Test 3
prog1=$(find "$(pwd)" -name driver1.out)
prog3=$(find "$(pwd)" -name driver3.out)
echo
echo driver1.c
$prog1 | sed 's/,,/, ,/g;s/,,/, ,/g' | column -t -s, -n
echo
echo driver3.c
$prog3 | sed 's/,,/, ,/g;s/,,/, ,/g' | column -t -s, -n 
