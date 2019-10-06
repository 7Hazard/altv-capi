cmake_minimum_required(VERSION 3.12)

#set(CMAKE_CXX_STANDARD 17)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -fms-extensions -fdelayed-template-parsing")
endif (UNIX)

include_directories(
    ./
    ../
)

add_definitions(
    -D_UNICODE
    -DUNICODE
)

# Client
add_library(altv-capi-client SHARED
    client/altv-capi.cpp
)
target_compile_definitions(altv-capi-client PRIVATE
    ALT_CLIENT_API
    CAPI_DLL
)
target_include_directories(altv-capi-client PRIVATE
    client
)

add_library(altv-capi-client-static STATIC
    client/altv-capi.cpp
)
target_compile_definitions(altv-capi-client-static PRIVATE
    ALT_CLIENT_API
)
target_include_directories(altv-capi-client-static PRIVATE
    client
)


# Server
add_library(altv-capi-server SHARED
    server/altv-capi.cpp
)
target_compile_definitions(altv-capi-server PRIVATE
    ALT_SERVER_API
    CAPI_DLL
)
target_include_directories(altv-capi-server PRIVATE
    server
)

add_library(altv-capi-server-static STATIC
    server/altv-capi.cpp
)
target_compile_definitions(altv-capi-server-static PRIVATE
    ALT_SERVER_API
)
target_include_directories(altv-capi-server-static PRIVATE
    server
)
