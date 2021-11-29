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

## To-Do

- General
  - [ ] Serialize `std::vector` and `std::map` (and other STL containers?)
  - [ ] Split `fire::h5::File` into the actual file and the event bus
  - [ ] Input file able to loop over multiple on-disk files as if one file
- More Testing
  - [ ] user class with container member variable
  - [ ] container of user class
  - [x] user class with user class member variable
- Port H5Easy file here and introduce our specializations
  - [ ] 1D buffering mechanism
  - [ ] high level of compression
  - [ ] chunking
- Move into Framework
  - [ ] Rename Framework -> fire
  - [ ] Pass names as an added layer of group
  - [ ] Read/Write RunHeaders
