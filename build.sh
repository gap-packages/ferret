#!/bin/sh -ex

./autogen.sh
./configure CXXFLAGS="-DHPCGAP -DCONFIG_H -DGC_THREADS -DBOEHM_GC -DUSE_GMP=1  -I../../src -I../../extern/64bit/include" "$@"
make

