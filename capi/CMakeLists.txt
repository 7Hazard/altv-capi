cmake_minimum_required(VERSION 3.12)

set (CMAKE_CXX_STANDARD 17)

include_directories(
    ./
    ../
)

add_definitions(
    -D_UNICODE
    -DUNICODE
)

# Client
add_library(altv-capi-client-dll SHARED
    client/altv-capi.cpp
)
target_compile_definitions(altv-capi-client-dll PRIVATE
    ALT_CLIENT_API
    CAPI_DLL
)

add_library(altv-capi-client STATIC
    client/altv-capi.cpp
)
target_compile_definitions(altv-capi-client PRIVATE
    ALT_CLIENT_API
)


# Server
add_library(altv-capi-server-dll SHARED
    server/altv-capi.cpp
)
target_compile_definitions(altv-capi-server-dll PRIVATE
    ALT_SERVER_API
    CAPI_DLL
)

add_library(altv-capi-server STATIC
    server/altv-capi.cpp
)
target_compile_definitions(altv-capi-server PRIVATE
    ALT_SERVER_API
)
