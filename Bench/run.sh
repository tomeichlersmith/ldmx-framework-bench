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
  printf "%s,%s,%s,%s,%s,%s,%s\n" $@
}

# input trials per n_events run and then squence of n_events to run
#  e.g. <script> <name> 100 1 10 100 1000 10000 100000
# we get the size of the output file by assuming it matches the form
#   output/*_<num-events>.*
__bench__() {
  local tag=$1; shift
  local trials=$1; shift
  [ -f data.csv ] || __print_csv_line__ runner writer reader mode events time size | tee data.csv
  local runner=$(__runner__)
  local n_events
  for n_events in $@; do
    echo "  benchmarking ${n_events} Events"
    local t=$(__time__ ${trials} ${tag}_produce.py ${n_events})
    local produce_output="output/output_${n_events}.${tag}"
    local s=$(stat -c "%s" ${produce_output})
    __print_csv_line__ ${runner} ${tag} ${tag} produce ${n_events} ${t} ${s} | tee -a data.csv
    t=$(__time__ ${trials} ${tag}_recon.py ${produce_output})
    s=$(stat -c "%s" output/recon_output_${n_events}.${tag}) 
    __print_csv_line__ ${runner} ${tag} ${tag} recon ${n_events} ${t} ${s} | tee -a data.csv
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
  bench [options] <trials> <n_events_1> [n_events_2 ...]

 ARGUMENTS:
  trails   : Number of trials to run for each N_EVENTS
  n_events : Number of events to benchmark for both ROOT and HDF5

 OPTIONS:
  -h, --help   : Print this help and exit
  --no-compile : Don't compile the two frameworks. Assume installations are available 
                 at ${LDMX_BASE}/install/root and ${LDMX_BASE}/install/hdf5.
  --no-clean   : Don't delete the output files (which are not intended for regular use)
  --no-interop : Don't test recon mode of HDF5 framework using ROOT files as inputs

HELP
}

bench() {
  if [ -z $1 ]; then
    __bench_help__
    return
  fi
  local _compile=true
  local _clean=true
  local _interop=true
  local _positional=()
  for arg in $@; do
    case $arg in
      --no-compile)
        _compile=false
        ;;
      --no-clean)
        _clean=false
        ;;
      --no-interop)
        _interop=false
        ;;
      -h|--help)
        __bench_help__
        return 0
        ;;
      -*)
        echo "Option $arg not recognized"
        return 1
        ;;
      *)
        _positional+=("$arg")
        ;;
    esac
  done

  __group__ Bench ROOT
  __group__ Init Environment
  ldmx use dev root-hdf5 || return $?
  if ${_compile}; then
    __endgroup__; __group__ Configure Build
    ldmx cmake -B build/root -S . -DCMAKE_INSTALL_PREFIX=install/root -DUSE_ROOT_FRAMEWORK=ON || return $?
    __endgroup__; __group__ Build and Install
    ldmx cmake --build build/root --target install || return $?
  fi
  ln -sfT install/root .container-install
  __endgroup__; __group__ Run Benchmark
  __bench__ root ${_positional[@]} || return $?
  __endgroup__;
  __endgroup__; __group__ Bench HDF5
  __group__ Init Environment
  ldmx use dev root-hdf5 || return $?
  if ${_compile}; then
    __endgroup__; __group__ Configure Build
    ldmx cmake -B build/hdf5 -S . -DCMAKE_INSTALL_PREFIX=install/hdf5 || return $?
    __endgroup__; __group__ Build and Install
    ldmx cmake --build build/hdf5 --target install || return $?
  fi
  ln -sfT install/hdf5 .container-install
  if ${_interop}; then
    local trials=${_positional[0]}
    local runner=$(__runner__)
    for n_events in ${_positional[@]:1}; do
      echo "  interop benchmarking ${n_events} Events"
      t=$(__time__ ${trials} hdf5_recon.py output/output_${n_events}.root)
      s=$(stat -c "%s" output/recon_output_${n_events}_root.hdf5) 
      __print_csv_line__ ${runner} root hdf5 recon ${n_events} ${t} ${s} | tee -a data.csv
    done
  fi
  __endgroup__; __group__ Run Benchmark
  __bench__ hdf5 ${_positional[@]} || return $?
  if ${_clean}; then
    __endgroup__; __group__ Delete Output Files
    rm -vr output || return $?
  fi
  __endgroup__
  return 0
}

