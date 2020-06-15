Clang tool for generating and building alt:V's C++ API into C FFI wrappings  

Download builds from Releases.  
Binaries for client-linux does not exist, cause the client itself does not exist as a linux binary.  

When using CAPI on windows, you must run the same CRT version as altv-server.exe or the client  
The build flag `/MD` should be used with Visual Studio and clang-cl for altv-server.exe

If you want to build the parser:
* Set-up LLVM
* Clone this repo into clang-tools-extra
* Add the repo as a project in clang-tools-extra's CMakeLists.txt
* Generate CMake files through the VS Code tasks
* Build through tasks and running configs in VS Code

Requirements (maybe there are more than specified):
* CMake 3.13+
* Clang
* Ninja
* (Windows) Visual Studio
