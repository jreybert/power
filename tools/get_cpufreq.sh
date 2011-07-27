#!/bin/bash


cpu_num=$(grep -c processor /proc/cpuinfo)

  for (( cpuid=0; cpuid<$cpu_num; cpuid++ ))
  do
    echo -n "CPU$cpuid "
    cpufreq-info -c $cpuid -p | cut -d ' ' -f3 | tr '\n' ' ' && cpufreq-info -fm
  done

