#!/bin/bash
# source file for bench mark runs
# assume we are running on GitHub action and from root directory of repo
# run this script inside the container and provide the number of trials 
# and then the number of events

# define helpful avg timeing function
#   source: https://stackoverflow.com/a/54920339/17617632
# usage: time <trials> <num-events>
#   removed tracking of user and sys times
#   just prints out the avg real time in seconds
#   assumes that the string 'real' does not appear in
#   output of 'fire'
__time__() {
  local n_trials=$1
  local n_events=$2
  for ((i = 0; i < n_trials; i++)); do
    time -p fire config.py ${n_events}
  done |& awk '/real/ { real = real + $2; nr++ }
    END { if (nr>0) printf("%f\n", real/nr); }'
}

# get the current branch name
#   assuming in GitHub workflow
__branch__() {
  echo ${GITHUB_REF#refs/heads/} 
}

# Print the five inputs into the five columns of a CSV line
__print_csv_line__() {
  printf "%s,%s,%s,%s,%s\n" $@
}

# input trials per n_events run and then squence of n_events to run
#  e.g. <script> 100 1 10 100 1000 10000 100000
# we get the size of the output file by assuming it matches the form
#   output/*_<num-events>.*
__main__() {
  local trials=$1; shift
  __print_csv_line__ runner serializer events time size > data.csv
  local br=$(__branch__)
  local n_events
  for n_events in $@; do
    echo "Benchmarking ${n_events} Events"
    local t=$(__time__ ${trials} ${n_events})
    local s=$(stat -c "%s" output/*_${n_events}.*)
    __print_csv_line__ docker ${br} ${n_events} ${t} ${s} >> data.csv
  done
}

__main__ $@
