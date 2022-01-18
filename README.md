# ldmx-rootless

<a href="https://github.com/tomeichlersmith/ldmx-rootless/actions" alt="Actions">
    <img src="https://github.com/tomeichlersmith/ldmx-rootless/workflows/CI/badge.svg" />
</a>

Trying to drag ldmx-sw into the 21st century by removing our dependency on the archaic ROOT.

## Anticipated Q&A
- Why?
  - ROOT fails to conform to modern software practices causing it to not be used outside of HEP.
    This means it is difficult to learn for new users, is poorly maintained, and has poor documentation.
  - Since ROOT's inception and standardization at CERN, modern industry big data tools have
    come into existence that are as performative, easier to use, and better documented than ROOT.
- ROOT is highly performative for our usecase, will we see reduced speed?
  - I have been developing a [benchmark test](Bench) comparing ROOT and HDF5 frameworks.
  - This test actually shows that the HDF5-based framework is five times faster than the ROOT framework.
- ROOT has long term-support, why not rely on that?
  - HDF5, matplotlib, pandas, and other industry tools are well supported by the wider (non-HEP) community,
    so we can safely rely on them to be maintained in the future. Moreover, since they have a wider user base,
    their development has surpassed ROOT in terms of stability and ease-of-use.
- ROOT allows for quick data analysis, why abandon it?
  - I disagree that ROOT allows for quick data analysis, especially for new users.
    ROOT is very opaque and difficult to understand, and I think you are underestimating how quick analysis can be with other tools.
  - One standard analysis technique is using `matplotlib` inside a Jupyter notebook.
    That is incredibly quick, flexible, and powerful.
- Translation layer for intermediate stage?
  - Absolutely possible to write a ROOT -> HDF5 converting tool.
    Is it useful? Our simulated data becomes stale (almost useless) pretty quickly and we end up deleting it.
    The container allows for re-generating older samples if need be.
- DAQ?
  - [HDF5 and h5py have a MPI interface](https://docs.h5py.org/en/stable/mpi.html) which could be used in conjunction with Rogue to write out data from multiple DAQ servers at once.
- Boost.Histogram
  - Already accessible, easy-to-use histogram filler following modern C++ coding practices
  - [More flexible and faster than ROOT or gsl](https://www.boost.org/doc/libs/1_78_0/libs/histogram/doc/html/histogram/benchmarks.html)
- Hours spent dealing with ROOT BS
  - Estimates over the lifetime of LDMX

## Environment Notes

A container image with HDF5, Boost, and HighFive installed has been built in the LDMX-Software/docker
repository on the hdf5 branch. I have copied over the `ldmx-env.sh` script here and modified it slightly
as well as copied over the `.ldmxrc` file and updated it to have the current directory be the base directory
and the default container image be the hdf5 image.

## Future Plans
- `fire` versioned and installed in developers container
- Jupyter notebook installed in dev container and port opened through env script
