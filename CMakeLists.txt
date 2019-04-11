cmake_minimum_required(VERSION 3.12)

set(LLVM_LINK_COMPONENTS support)
set(LLVM_ENABLE_CXX1Z ON)

add_clang_executable(altv-api-tool
    src/main.cpp
)
target_link_libraries(altv-api-tool PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)
