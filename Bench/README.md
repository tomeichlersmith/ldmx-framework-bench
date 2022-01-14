# Bench

Benchmark testing between ROOT-based fire and HDF5-based fire.

The actual compiling and running of this module is done in the
[benchmark GitHub action](../.github/workflows/bench.yml).

In short, after compiling whichever version of the framework we are
using against this module, we run the command

```bash
ldmx time fire config.py <num-events>
```

To time how long the processing takes, and then

```bash
stat output/*
```

To obtain how large the output data file is.

[Latest Results](results)

## Run Locally

If you want to run this benchmarking test on your own machine, you can use the script that the GitHub workflow uses.
The following three commands compile, install and benchmark whatever version of the framework and the bench module
are currently checked out in this repository.
The last command is what specifies how many trials to run per event number bench mark point,
which event number points to use, and what to label this run.

```bash
ldmx cmake -B build -S .
ldmx cmake --build build --target install
ldmx ./.github/workflows/bench.sh <label> 100 1 10 100 1000 10000 100000 1000000
```

Outlining the full benchmarking process from having nothing on your computer.
```bash
git clone --recursive https://github.com/tomeichlersmith/ldmx-rootless.git
cd ldmx-rootless
source rootless-env.sh
# do HDF5 based compiling and benchmark
git checkout hdf5
ldmx use dev hdf5 # choose container with HDF5
ldmx cmake -B build -S .
ldmx cmake --build build --target install
ldmx ./.github/workflows/bench.sh hdf5 100 1 10 100 1000 10000 100000 1000000
# cleanup
rm -r build .container-install output
# do ROOT based compiling and benchmark
git checkout root
git submodule update
ldmx use dev v3.1 # choose container with ROOT
ldmx cmake -B build -S .
ldmx cmake --build build --target install
ldmx ./.github/workflows/bench.sh root 100 1 10 100 1000 10000 100000 1000000
```
