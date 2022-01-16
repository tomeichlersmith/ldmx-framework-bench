# source file for bench mark runs
# assume we are running from root directory of repo
# run this script OUTSIDE the container and provide the number of trials 
# and then the number of events

if ! hash ldmx &> /dev/null; then
  echo "Not inside ldmx environmnet!"
  return 1
fi

# define helpful avg timeing function
#   source: https://stackoverflow.com/a/54920339/17617632
# usage: time <trials> <fire-args>
#   removed tracking of user and sys times
#   just prints out the avg real time in seconds
#   assumes that the string 'real' does not appear in
#   output of 'fire'
__time__() {
  local n_trials=$1
  for ((i = 0; i < n_trials; i++)); do
    ldmx time -p fire ${@:2} || return $?
  done |& awk '/real/ { real = real + $2; nr++ }
    END { if (nr>0) printf("%f\n", real/nr); }'
}

__runner__() {
  [ -f /.dockerenv ] && { echo "docker"; return 0; }
  [ -f /singularity ] && { echo "singularity"; return 0; }
  echo "bare"
  return 0
}

# Print the five inputs into the five columns of a CSV line
__print_csv_line__() {
  printf "%s,%s,%s,%s,%s,%s\n" $@
}

# input trials per n_events run and then squence of n_events to run
#  e.g. <script> <name> 100 1 10 100 1000 10000 100000
# we get the size of the output file by assuming it matches the form
#   output/*_<num-events>.*
__bench__() {
  local tag=$1; shift
  local trials=$1; shift
  [ -f data.csv ] || __print_csv_line__ runner serializer mode events time size | tee data.csv
  local runner=$(__runner__)
  local n_events
  for n_events in $@; do
    echo "Benchmarking ${n_events} Events"
    local t=$(__time__ ${trials} ${tag}_produce.py ${n_events})
    [[ "$?" != "0" ]] && { echo "fire produce.py Errored Out!"; return 1; }
    local produce_output="output/output_${n_events}"
    local s=$(stat -c "%s" ${produce_output})
    __print_csv_line__ ${runner} ${tag} produce ${n_events} ${t} ${s} | tee -a data.csv
    t=$(__time__ ${trials} ${tag}_recon.py ${produce_output})
    [[ "$?" != "0" ]] && { echo "fire recon.py Errored Out!"; return 1; }
    s=$(stat -c "%s" output/recon_output_${n_events}) 
    __print_csv_line__ ${runner} ${tag} recon ${n_events} ${t} ${s} | tee -a data.csv
  done
}

__group__() {
  echo "::group::$@"
}

__endgroup__() {
  echo "::endgroup::"
}

__bench_help__() {
  cat << HELP

 USAGE:
  bench <trials> <n_events_1> [n_events_2 ...]

 ARGUMENTS:
  trails   : Number of trials to run for each N_EVENTS
  n_events : Number of events to benchmark for both ROOT and HDF5

HELP
}

bench() {
  if [ -z $1 ]; then
    __bench_help__
    return
  fi
  __group__ Bench ROOT
  __group__ Init Environment
  ldmx use dev latest || return $?
  __endgroup__; __group__ Configure Build
  ldmx cmake -B build/root -S . -DUSE_ROOT=ON || return $?
  __endgroup__; __group__ Build and Install
  ldmx cmake --build build/root --target install || return $?
  __endgroup__; __group__ Run Benchmark
  __bench__ root $@ || return $?
  __endgroup__;
  __endgroup__; __group__ Bench HDF5
  __group__ Init Environment
  ldmx use dev hdf5 || return $?
  __endgroup__; __group__ Configure Build
  ldmx cmake -B build/hdf5 -S . -DUSE_ROOT=OFF || return $?
  __endgroup__; __group__ Build and Install
  ldmx cmake --build build/root --target install || return $?
  __endgroup__; __group__ Run Benchmark
  __bench__ hdf5 $@ || return $?
  local rc=$?
  __endgroup__
  return ${rc}
}

