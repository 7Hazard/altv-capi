Clang tool for parsing, building and working with alt:V's cpp-api

To get the latest distribution via URL:  
```https://gitlab.com/7Hazard/altv-capi/-/jobs/artifacts/<ref>/raw/capi/BUILD[-WIN32]/altv-capi-<server/client>-<platform>[-static].zip?job=publish&inline=false```

* `<ref>` is the branch/tag
* `[-WIN32]` is `-WIN32` when platform is win32
* `<server/client>` is `server` or `client`
* `<platform>` is either `win32` or `linux`
* `[-static]` is `-static` when you want static link binaries

For example, to download dynamic CAPI server binaries for win32:  
[https://gitlab.com/7Hazard/altv-capi/-/jobs/artifacts/master/raw/capi/BUILD-WIN32/altv-capi-server-win32.zip?job=publish&inline=false](https://gitlab.com/7Hazard/altv-capi/-/jobs/artifacts/master/raw/capi/BUILD-WIN32/altv-capi-server-win32.zip?job=publish&inline=false)

Binaries for client-linux does not exist.  
**NOTE: If the download to latest refers to a 404, it is because the latest build failed!**  
Either chose an older successful build from Releases (report if necessary).

If you want to build the parser:
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
