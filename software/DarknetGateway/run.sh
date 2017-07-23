#!/bin/bash
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/src/tbb/build/linux_armv7_gcc_cc4.9.2_libc2.19_kernel4.9.28_release ./darknet-gateway.bin

