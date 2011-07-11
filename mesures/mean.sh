#!/bin/bash

echo $1
awk '{ s += $1 } END { print " average: ", s/NR}' $1
