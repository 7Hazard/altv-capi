#!/bin/bash

cmake . -BBUILD-WIN32 -DCMAKE_BUILD_TYPE=RelWithDebInfo -GNinja -DCMAKE_TOOLCHAIN_FILE='../cmake/x64_win32.cmake' && cmake --build BUILD-WIN32 --config RelWithDebInfo

#zip -j altv-capi-server-win32.zip capi/BUILD-WIN32/altv-capi-server-static.lib
