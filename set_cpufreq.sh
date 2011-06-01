#!/bin/bash

parameter=$1

cpu_num=$(grep -c processor /proc/cpuinfo)

min_freq=$(cpufreq-info -l | cut -f1 -d' ')
max_freq=$(cpufreq-info -l | cut -f2 -d' ')

if [ $parameter == "max" ]
then
  echo "Set to max"
  for (( cpuid=0; cpuid<$cpu_num; cpuid++ ))
  do
    echo -n "CPU$cpuid "
    sudo cpufreq-set -c $cpuid -g userspace
    sudo cpufreq-set -c $cpuid -f $max_freq
  done
elif [  $parameter = "min" ]
then
  echo "Set to min"
  for (( cpuid=0; cpuid<$cpu_num; cpuid++ ))
  do
    echo -n "CPU$cpuid "
    sudo cpufreq-set -c $cpuid -g userspace
    sudo cpufreq-set -c $cpuid -f $min_freq
  done
elif [ $parameter = "auto" ]
then
  echo "Set to auto"
  for (( cpuid=0; cpuid<$cpu_num; cpuid++ ))
  do
    echo -n "CPU$cpuid "
    sudo cpufreq-set -c $cpuid -g ondemand
  done
else
  echo "Usage: $0 [max|min|auto]"
fi

echo ""


