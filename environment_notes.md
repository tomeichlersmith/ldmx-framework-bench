
# Notes on Environment

## Operating System

```bash
$ cat /etc/os-release
NAME="CentOS Linux"
VERSION="7 (Core)"
ID="centos"
ID_LIKE="rhel fedora"
VERSION_ID="7"
PRETTY_NAME="CentOS Linux 7 (Core)"
ANSI_COLOR="0;31"
CPE_NAME="cpe:/o:centos:centos:7"
HOME_URL="https://www.centos.org/"
BUG_REPORT_URL="https://bugs.centos.org/"

CENTOS_MANTISBT_PROJECT="CentOS-7"
CENTOS_MANTISBT_PROJECT_VERSION="7"
REDHAT_SUPPORT_PRODUCT="centos"
REDHAT_SUPPORT_PRODUCT_VERSION="7"
```

## Compiler
gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

## HDF5

1. Download Source Code from [website](https://www.hdfgroup.org/downloads/hdf5/source-code/#)
2. Unpack (they have packed it in a way where `tar xzf` doesnt work on my computer).
```bash
gunzip < hdf5-1.12.1.tar.gz | tar xf - 
```
NB: Look at release_docs/INSTALL for more documentation on building from source.
3. Configure for Local Install (and standard C++ interface just in case)
```
cd hdf5-1.12.1
./configure --prefix=$(pwd)/install --enable-cxx
```
4. Build (only like 10 minutes! lmao)
```
make -j2
```
5. Check (longer than the build!!)
```
make check
```
6. Install
```
make install
```
7. Verify Install
```
make check-install
```

## CMake
The cmake that comes with my operating system is super old, so I am upgrading it within my users python.
```
python3 -m pip install --user --upgrade cmake
export PATH=$HOME/.local/bin:${PATH}
```
Now I have cmake version 3.21.4.

## HighFive

1. `git` source
```
git clone https://github.com/BlueBrain/HighFive
git checkout v2.3.1 # go to most recent release
```
2. Configure to Install Alongside HDF5 for easier linking
```
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=../hdf5-1.12.1/install
```
3. "Build" and Install
We build tests by default, we want to do this "build" so that a cmake config file is generated.
```
cd build
make install
```

