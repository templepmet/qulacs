#!/bin/bash -eu

# rm -rf build
USE_TEST="Yes" ./script/build_gcc_with_gpu.sh
cd build
make test
make pythontest
