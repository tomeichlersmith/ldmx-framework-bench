FROM ldmx/dev:hdf5

# build and install
COPY . /src/
RUN cmake \
      -DCMAKE_INSTALL_PREFIX=${__prefix} \
      -B /src/build \
      -S /src &&\
    cmake \
      --build /src/build \
      --target install &&\
    rm -rf /src/ &&\
    ldconfig
ENV PYTHONPATH ${__prefix}/python
