#!/bin/bash

file=summary

rm -f $file

for i in *
do
  echo $i >> $file
  tail -n+10 $i | awk '{ s += $1 } END { print " average: ", s/NR}' >> $file
done
