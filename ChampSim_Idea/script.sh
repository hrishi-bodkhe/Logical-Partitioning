#!/bin/bash

# Command to run
cmd="./run_champsim.sh bimodal-no-no-no-no-lru-1core 100 200"

# List of parameters
parameters=("437.leslie3d-134B.champsimtrace.xz" "462.libquantum-714B.champsimtrace.xz" "619.lbm_s-2676B.champsimtrace.xz" "429.mcf-184B.champsimtrace.xz" "401.bzip2-7B.champsimtrace.xz" "456.hmmer-88B.champsimtrace.xz" "450.soplex-247B.champsimtrace.xz" "astar_23B.trace.xz" "444.namd-44B.champsimtrace.xz")

# Loop through the parameters and run the command with each parameter
for param in "${parameters[@]}"
do
  # Add the current parameter to the command
  full_cmd="$cmd $param"
  
  # Run the command
  eval $full_cmd
done
