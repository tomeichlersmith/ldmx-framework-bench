#!/bin/bash
# source file for bench mark runs
# assume we are running on GitHub action and from root directory of repo
# run this script inside the container and provide the number of trials 
# and then the number of events

# define helpful avg timeing function
#   source: https://stackoverflow.com/a/54920339/17617632
# usage: avg_time <trials> command ...
__avg_time__() {
  local n=$1; shift
  (( $# >0 )) || return # bail if no command give
  for ((i = 0; i < n; i++)); do
    { time -p "$@" &>/dev/null; } 2>&1 # ignore the output of the command
      # but collect time's output in stdout
    done | awk '
    /real/ { real = real + $2; nr++ }
    /user/ { user = user + $2; nu++ }
    /sys/  { sys  = sys  + $2; ns++}
    END    {
    if (nr>0) printf("real %f\n", real/nr);
      if (nu>0) printf("user %f\n", user/nu);
        if (ns>0) printf("sys %f\n",  sys/ns)
        }'
}

__avg_time__ $1 fire config.py $2
