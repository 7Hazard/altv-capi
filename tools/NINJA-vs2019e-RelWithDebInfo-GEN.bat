:: Buildscript for building on windows with Ninja and clang-cl
:: Copy the script to llvm-project root dir
:: VISUAL STUDIO 2019 ENTERPRISE

set "curdir=%cd%"

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall" amd64
cd /d %curdir%

set CFLAGS=-m64
set CXXFLAGS=-m64

cmake -G Ninja llvm -BA-BUILD-NINJA-VS2019-RelWithDebInfo -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DLLVM_ENABLE_CXX1Z=ON -DLLVM_BUILD_TESTS=OFF -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files/LLVM/bin/lld-link.exe"
