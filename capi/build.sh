#!/bin/bash

echo "GENERATING CLIENT CODE"
./gen-client.sh
echo "GENERATING SERVER CODE"
./gen-server.sh

echo "BUILDING WRAPPERS"
cmake . -BBUILD -DCMAKE_BUILD_TYPE=RelWithDebInfo -GNinja -DCMAKE_C_COMPILER='clang-9' -DCMAKE_CXX_COMPILER='clang++-9' -DCMAKE_LINKER='ld.lld-9'
cmake --build BUILD --config RelWithDebInfo
