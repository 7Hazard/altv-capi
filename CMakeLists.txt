cmake_minimum_required(VERSION 3.10)

set(LLVM_LINK_COMPONENTS support)
set(LLVM_ENABLE_CXX1Z ON)

include_directories(
    src
)

add_clang_executable(altv-capi-gen
    src/main.cpp
    
    src/enum.cpp
    src/templateClass.cpp
    # src/templateAlias.cpp
    # src/typedef.cpp
    src/access.cpp
    src/structs.cpp
    src/methods.cpp
)
target_link_libraries(altv-capi-gen PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
)
