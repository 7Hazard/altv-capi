#!/bin/bash

export RC=/usr/bin/llvm-rc-9
export rc=/usr/bin/llvm-rc-9
export MT=/usr/bin/llvm-mt-9
export mt=/usr/bin/llvm-mt-9

echo "BUILDING CAPI"
cmake . -BBUILD-WIN32 -DCMAKE_BUILD_TYPE=RelWithDebInfo -GNinja -DCMAKE_TOOLCHAIN_FILE='../cmake/x64_win32.cmake' -DCWIN_LLVM_VERSION=9
cmake --build BUILD-WIN32 --config RelWithDebInfo

#zip -j altv-capi-server-win32.zip capi/BUILD-WIN32/altv-capi-server-static.lib
