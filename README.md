Clang tool for parsing and working with alt:V's cpp-api

If you want to build the parser
* Set-up LLVM
* Clone this repo into clang-tools-extra
* Add the repo as a project in clang-tools-extra CMakeLists.txt
* (Windows only) Move the bat scripts from tools into LLVM root
* Generate CMake files through the VS Code tasks
* Build through tasks and running configs in VS Code

Requirements:
* CMake
* Clang
* Ninja
