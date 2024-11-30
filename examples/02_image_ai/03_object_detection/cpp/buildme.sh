#!/bin/bash

clear

## NOTE: use the following only for cross-compile
#declare -A COMPILER=( [x86_64]=/usr/bin/gcc
#                      [aarch64]=/usr/bin/aarch64-linux-gnu-gcc
#                      [armv7l]=/usr/bin/arm-linux-gnueabi-gcc )

export CMAKE_BUILD_PARALLEL_LEVEL=4

for ARCH in aarch64
do
    echo "-I- Building ${ARCH}"
    mkdir -p build/${ARCH}
    cmake -H. -Bbuild/${ARCH}
    cmake --build build/${ARCH}  -j 4
done
if [[ -f "hailort.log" ]]; then
    rm hailort.log
fi
