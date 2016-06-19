#!/bin/sh
rm -r CMake*
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../zynq.cmake -G "Unix Makefiles" ..
