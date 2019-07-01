cmake_minimum_required(VERSION 3.12)

set (CMAKE_CXX_STANDARD 17)

include_directories(
    ../../
)

add_library(altv-client-capi-dll SHARED
    altv-capi.cpp
)

target_compile_definitions(altv-client-capi-dll PRIVATE
    ALT_CLIENT_API
    CAPI_DLL
    
    _UNICODE
    UNICODE
)

add_library(altv-client-capi STATIC
    altv-capi.cpp
)

target_compile_definitions(altv-client-capi PRIVATE
    ALT_CLIENT_API
    
    _UNICODE
    UNICODE
)
