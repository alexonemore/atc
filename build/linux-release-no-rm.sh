#!/bin/bash

#rm -rf build-linux &&\
#mkdir build-linux &&\
cd build-linux &&\
cmake ../../ -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=install/ &&\
cmake --build . --config Release --parallel --target install
cd install/bin
./atc
