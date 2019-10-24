cmake_minimum_required(VERSION 3.12)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -fms-extensions -fdelayed-template-parsing -fexceptions")
endif()

include_directories(
    ./
    ../
)

add_definitions(
    -D_UNICODE
    -DUNICODE
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Client (only Windows)
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
    set_target_properties(altv-capi-client PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client/lib
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client/lib
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client/bin
    )

    # Client static
    add_library(altv-capi-client-static STATIC
        client/altv-capi.cpp
    )
    target_compile_definitions(altv-capi-client-static PRIVATE
        ALT_CLIENT_API
    )
    target_include_directories(altv-capi-client-static PRIVATE
        client
    )
    set_target_properties(altv-capi-client-static PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client-static/lib
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client-static/lib
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client-static/bin
    )
endif()



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
set_target_properties(altv-capi-server PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server/lib
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server/lib
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server/bin
)

# Server static
add_library(altv-capi-server-static STATIC
    server/altv-capi.cpp
)
target_compile_definitions(altv-capi-server-static PRIVATE
    ALT_SERVER_API
)
target_include_directories(altv-capi-server-static PRIVATE
    server
)
set_target_properties(altv-capi-server-static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server-static/lib
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server-static/lib
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server-static/bin
)
