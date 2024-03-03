#!/bin/bash

# Update package list
sudo apt-get update

# Install dependencies
sudo apt-get install -y build-essential git sudo pkg-config ccache python3-pip \
    valgrind libfmt-dev gcc-11 g++-11 ninja-build ragel libhwloc-dev libnuma-dev \
    libpciaccess-dev libcrypto++-dev libboost-all-dev libxml2-dev xfslibs-dev \
    libgnutls28-dev liblz4-dev libsctp-dev gcc make libprotobuf-dev \
    protobuf-compiler python3 systemtap-sdt-dev libtool cmake libyaml-cpp-dev \
    libc-ares-dev stow libssl-dev liburing-dev libgflags-dev libuv1-dev \
    libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev
