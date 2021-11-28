# ldmx-rootless

<a href="https://github.com/tomeichlersmith/ldmx-rootless/actions" alt="Actions">
    <img src="https://github.com/tomeichlersmith/ldmx-rootless/workflows/CI/badge.svg" />
</a>

Trying to drag ldmx-sw into the 21st century by removing our dependency on the archaic ROOT.

## Environment Notes

A container image with HDF5, Boost, and HighFive installed has been built in the LDMX-Software/docker
repository on the hdf5 branch. I have copied over the `ldmx-env.sh` script here and modified it slightly
as well as copied over the `.ldmxrc` file and updated it to have the current directory be the base directory
and the default container image be the hdf5 image.
