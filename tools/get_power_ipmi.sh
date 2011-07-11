#!/bin/bash

if [ $# -lt 2 ]
then
  echo "Usage: $0 out_file nb_secs command"
  exit
fi

out_file=$1
shift
nb_secs=$1
shift

exec $@ &

rm -f $out_file

for (( i=1; i<=$nb_secs; i++ ))
do
  #echo -n -e "\r$i / $nb_secs"
  sudo ipmi-oem Dell get-instantaneous-power-consumption-data 0 | head -n 1 | cut -f4 -d' ' >> $out_file
  sleep 1s
done

killall $1
