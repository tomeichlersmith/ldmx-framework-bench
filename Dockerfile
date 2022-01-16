FROM ldmx/dev:v3.1

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
ENV LDMX_SW_INSTALL ${__prefix}