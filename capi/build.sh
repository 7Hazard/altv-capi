#!/bin/bash

cmake . -BBUILD -DCMAKE_BUILD_TYPE=RelWithDebInfo -GNinja -DCMAKE_C_COMPILER='clang' -DCMAKE_CXX_COMPILER='clang++' -DCMAKE_LINKER='ld.lld' && cmake --build BUILD --config RelWithDebInfo
