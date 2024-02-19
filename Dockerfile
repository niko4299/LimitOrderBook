FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get -qq update -y && apt-get -qq dist-upgrade -y
RUN apt install -y build-essential git sudo pkg-config ccache python3-pip \
    valgrind libfmt-dev gcc-11 g++-11 ninja-build ragel libhwloc-dev libnuma-dev libpciaccess-dev libcrypto++-dev libboost-all-dev \
    libxml2-dev xfslibs-dev libgnutls28-dev liblz4-dev libsctp-dev gcc make libprotobuf-dev protobuf-compiler python3 systemtap-sdt-dev \
    libtool cmake libyaml-cpp-dev libc-ares-dev stow libssl-dev liburing-dev libgflags-dev libuv1-dev

WORKDIR /app

# Remove this after merge to master
COPY . /app

# Uncomment the following when merged to master
# RUN git clone https://github.com/niko4299/UnlimitedOrderBook.git
# WORKDIR /app/UnlimitedOrderBook

WORKDIR /app/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release && make -j6

EXPOSE 8000/tcp
ENTRYPOINT ./UnlimitedOrderBook
