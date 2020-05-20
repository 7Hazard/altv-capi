:: Buildscript for building on windows with Ninja and clang-cl
:: Copy the script to llvm-project root dir

set "curdir=%cd%"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall" amd64
cd /d %curdir%

set CFLAGS=-m64
set CXXFLAGS=-m64

cmake --build A-BUILD-NINJA-RelWithDebInfo --config RelWithDebInfo --target altv-api-tool
