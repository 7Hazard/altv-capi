# alt:V C API

Clang tools and binaries for generating and building alt:V's C++ API into C FFI wrappings  

Download builds from Releases.  
Binaries for client-linux does not exist, cause the client itself does not exist as a linux binary.  

When using CAPI on windows, you must run the same CRT version as altv-server.exe or the client  
The build flag `/MD` should be used with Visual Studio and clang-cl for altv-server.exe

## Requirements for building (maybe there are more than specified)
* CMake 3.13+
* Clang
* Ninja
* (Windows) Visual Studio

## Building the generator
* Set-up LLVM
* Clone this repo into clang-tools-extra
* Add the repo as a project in clang-tools-extra's CMakeLists.txt
* Generate CMake files through the VS Code tasks
* Build through tasks and running configs in VS Code

## Building CAPI locally
Open VS Code from project root if you want to use VS Code  
* Run `./tools/get-gen` from project root (if you're not building the generator locally)
* Run `./tools/get-cppsdk` from project root (if you're not building the generator locally)
* Run `./gen-client` from inside `capi` folder, or via VS Code tasks
* Run `./gen-server` from inside `capi` folder, or via VS Code tasks
* Run one of the build scripts from inside `capi` folder, or via VS Code tasks

The outputs will be in `client`, `server` and a `BUILD` folder
