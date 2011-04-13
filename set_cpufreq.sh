#!/bin/bash

parameter=$1
if [ $parameter == "max" ]
then
  echo "Set to max"
  sudo cpufreq-set -c 0 -g userspace
  sudo cpufreq-set -c 0 -f 1833000
  sudo cpufreq-set -c 1 -g userspace
  sudo cpufreq-set -c 1 -f 1833000
elif [  $parameter = "min" ]
then
  echo "Set to min"
  sudo cpufreq-set -c 0 -g userspace
  sudo cpufreq-set -c 0 -f 1000000
  sudo cpufreq-set -c 1 -g userspace
  sudo cpufreq-set -c 1 -f 1000000
elif [ $parameter = "auto" ]
then
  echo "Set to auto"
  sudo cpufreq-set -c 0 -g ondemand
  sudo cpufreq-set -c 1 -g ondemand
else
  echo "Usage: $0 [max|min|auto]"
fi


