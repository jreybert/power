#!/bin/bash

i=0
mean=0
sum=0
top=0
echo "Energy consumption"

while true
do
  instant_cons=$(sudo ipmi-oem Dell get-instantaneous-power-consumption-data 0 | head -n 1 | cut -f4 -d' ')
  if [[ $instant_cons -gt $top ]]; then
    top=$instant_cons
  fi
  i=$(($i+1))
  sum=$(($sum+$instant_cons))
  mean=$(($sum/$i))
  echo -n -e "\rInstant: $instant_cons W - Mean: $mean W - Top: $top W"
  sleep 1s
done
