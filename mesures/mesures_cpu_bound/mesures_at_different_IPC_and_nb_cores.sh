#!/bin/bash

# this script launch different executables, each exe have different behaviour (CPI)
# here a short description
# HFM
# 6  Gop/s cpu_bound_O0 1
# 20 Gop/s cpu_bound_O0 2
# 30 Gop/s cpu_bound_O3 2
# 42 Gop/s cpu_bound_O3 3


bound_O0="tests/cycle_count/cpu_bound_O0"
bound_O3="tests/cycle_count/cpu_bound_O3"
mull_opt="tests/cache_miss/mull-opt 10240"
mull_unopt="tests/cache_miss/mull-unopt 10240"

get_ipmi_cmd=./get_power_ipmi.sh

burn_time=30
pause_time=5


global_prefix="mesures_membound/"

verbose="-report-bindings"

mkdir -p ${global_prefix}/min/L2/ ${global_prefix}/min/LLC/ 
mkdir -p ${global_prefix}/max/L2/ ${global_prefix}/max/LLC/ 


function burn {
# parameters
# 1. prefix
# 2. mpirun options
  local_prefix=$1
  mpi_opt=$2
  min_max=$3
  echo "== $local_prefix =="
  echo "   mull-opt"
  ./get_power_ipmi.sh ${global_prefix}/$min_max/L2/${local_prefix} $burn_time mpirun $verbose $mpi_opt $mull_opt
  sleep $pause_time
  echo "   mull-unopt"
  ./get_power_ipmi.sh ${global_prefix}/$min_max/LLC/${local_prefix} $burn_time mpirun $verbose $mpi_opt $mull_unopt
  sleep $pause_time
}

set_cpufreq max

# == 1 task ==
# 1 process 1 socket
burn "01task_01socket_01corepersoc" "-np 1 -bycore -bind-to-socket" "max"

# == 2 task ==
# 2 process per socket
burn "02task_01socket_02corepersoc" "-np 2 -bycore -bind-to-socket" "max"
# 1 process per socket
burn "02task_02socket_01corepersoc" "-np 2 -bysocket -bind-to-socket" "max"

# == 4 task ==
# 4 process per socket
burn "04task_01socket_04corepersoc" "-np 4 -bycore -bind-to-socket" "max"
# 1 process per socket
burn "04task_04socket_01corepersoc" "-np 4 -bysocket -bind-to-socket" "max"
# 2 process per socket
burn "04task_02socket_02corepersoc" "-np 4 -cpus-per-proc 4 -bind-to-socket" "max"

# == 8 task ==
# 8 process per socket
burn "08task_01socket_08corepersoc" "-np 8 -bycore -bind-to-socket" "max"
# 2 process per socket
burn "08task_04socket_02corepersoc" "-np 8 -bysocket -bind-to-socket" "max"
# 4 process per socket
burn "08task_02socket_04corepersoc" "-np 8 -cpus-per-proc 2 -bind-to-socket" "max"

# == 16 task ==
# 8 process per socket
burn "16task_02socket_08corepersoc" "-np 16 -bycore -bind-to-socket" "max"
# 4 process per socket
burn "16task_04socket_04corepersoc" "-np 16 -bysocket -bind-to-socket" "max"

# == 32 task ==
# 8 process per socket
burn "32task_04socket_08corepersoc" "-np 32 -bycore -bind-to-socket" "max"




set_cpufreq min

# == 1 task ==
# 1 process 1 socket
burn "01task_01socket_01corepersoc" "-np 1 -bycore -bind-to-socket" "min"

# == 2 task ==
# 2 process per socket
burn "02task_01socket_02corepersoc" "-np 2 -bycore -bind-to-socket" "min"
# 1 process per socket
burn "02task_02socket_01corepersoc" "-np 2 -bysocket -bind-to-socket" "min"

# == 4 task ==
# 4 process per socket
burn "04task_01socket_04corepersoc" "-np 4 -bycore -bind-to-socket" "min"
# 1 process per socket
burn "04task_04socket_01corepersoc" "-np 4 -bysocket -bind-to-socket" "min"
# 2 process per socket
burn "04task_02socket_02corepersoc" "-np 4 -cpus-per-proc 4 -bind-to-socket" "min"

# == 8 task ==
# 8 process per socket
burn "08task_01socket_08corepersoc" "-np 8 -bycore -bind-to-socket" "min"
# 2 process per socket
burn "08task_04socket_02corepersoc" "-np 8 -bysocket -bind-to-socket" "min"
# 4 process per socket
burn "08task_02socket_04corepersoc" "-np 8 -cpus-per-proc 2 -bind-to-socket" "min"

# == 16 task ==
# 8 process per socket
burn "16task_02socket_08corepersoc" "-np 16 -bycore -bind-to-socket" "min"
# 4 process per socket
burn "16task_04socket_04corepersoc" "-np 16 -bysocket -bind-to-socket" "min"

# == 32 task ==
# 8 process per socket
burn "32task_04socket_08corepersoc" "-np 32 -bycore -bind-to-socket" "min"

set_cpufreq auto

