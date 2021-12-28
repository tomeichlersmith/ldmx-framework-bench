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
