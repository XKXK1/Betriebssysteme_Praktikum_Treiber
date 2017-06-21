#!/bin/bash

destination="/dev/chardev"

cat < $destination
echo "ab" > $destination
cat < $destination
sleep 2.5
echo "abc" > $destination
cat < $destination
sleep 1.195
echo "abc1" > $destination
cat < $destination
sleep 0.2
echo "abc12" > $destination
cat < $destination
sleep 7
echo "abc12 " > $destination
cat < $destination
sleep 3.8
echo "John Cena is not alive" > $destination
cat < $destination

dmesg | grep Chardev | tail --lines=56
