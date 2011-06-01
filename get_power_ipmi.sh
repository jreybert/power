#!/bin/bash

if [ $# -ne 2 ]
then
  echo "Usage: $0 out_file nb_secs"
  exit
fi

out_file=$1
nb_secs=$2

echo -n "" > $1

for (( i=1; i<=$nb_secs; i++ ))
do
  echo -n -e "\r$i / $nb_secs"
  sudo ipmi-oem Dell get-instantaneous-power-consumption-data 0 | head -n 1 | cut -f4 -d' ' >> $1
  sleep 1s
done
