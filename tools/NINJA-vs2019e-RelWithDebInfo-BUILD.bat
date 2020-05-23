:: Buildscript for building on windows with Ninja and clang-cl
:: Copy the script to llvm-project root dir
:: VISUAL STUDIO 2019 ENTERPRISE

set "curdir=%cd%"

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall" amd64
cd /d %curdir%

set CFLAGS=-m64
set CXXFLAGS=-m64

cmake --build A-BUILD-NINJA-VS2019-RelWithDebInfo --config RelWithDebInfo --target altv-api-tool
