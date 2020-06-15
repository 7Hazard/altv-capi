cmake_minimum_required(VERSION 3.10)
project(altv-capi)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -fms-extensions -fdelayed-template-parsing -fexceptions")
endif()

file(GLOB_RECURSE CAPI_FILES_WATCH
    "../cpp-sdk/**/*.h"
    "../cpp-sdk/**/*.hpp"
    "../src/**/*.h"
    "../src/**/*.hpp"
    "../src/**/*.cpp"
)

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
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/client/altv-capi.cpp
        IMPLICIT_DEPENDS CXX ${CAPI_FILES_WATCH}
        COMMAND gen-client
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    add_library(altv-capi-client SHARED
        ${CMAKE_SOURCE_DIR}/altv-capi-client.cpp
    )
    target_compile_definitions(altv-capi-client PRIVATE
        ALT_CLIENT_API
        CAPI_DLL
    )
    target_include_directories(altv-capi-client PRIVATE

    )
    set_target_properties(altv-capi-client PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client/lib
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client/lib
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client/bin
    )
    add_custom_command(TARGET altv-capi-client POST_BUILD # POST BUILD COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/client/altv-capi.h     ${PROJECT_BINARY_DIR}/altv-capi-client/include/client/altv-capi.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-client.h     ${PROJECT_BINARY_DIR}/altv-capi-client/include/altv-capi-client.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-predefines.h ${PROJECT_BINARY_DIR}/altv-capi-client/include/altv-capi-predefines.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-extra.h      ${PROJECT_BINARY_DIR}/altv-capi-client/include/altv-capi-extra.h
    )

    # Client static
    add_library(altv-capi-client-static STATIC
        ${CMAKE_SOURCE_DIR}/altv-capi-client.cpp
    )
    target_compile_definitions(altv-capi-client-static PRIVATE
        ALT_CLIENT_API
    )
    target_include_directories(altv-capi-client-static PRIVATE

    )
    set_target_properties(altv-capi-client-static PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client-static/lib
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client-static/lib
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-client-static/bin
        POSITION_INDEPENDENT_CODE ON
    )
    add_custom_command(TARGET altv-capi-client-static POST_BUILD # POST BUILD COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/client/altv-capi.h     ${PROJECT_BINARY_DIR}/altv-capi-client-static/include/client/altv-capi.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-client.h     ${PROJECT_BINARY_DIR}/altv-capi-client-static/include/altv-capi-client.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-predefines.h ${PROJECT_BINARY_DIR}/altv-capi-client-static/include/altv-capi-predefines.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-extra.h      ${PROJECT_BINARY_DIR}/altv-capi-client-static/include/altv-capi-extra.h
    )
endif()



# Server
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/server/altv-capi.cpp
        IMPLICIT_DEPENDS CXX ${CAPI_FILES_WATCH}
        COMMAND gen-server
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/server/altv-capi.cpp
        IMPLICIT_DEPENDS CXX ${CAPI_FILES_WATCH}
        COMMAND ./gen-server.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
add_library(altv-capi-server SHARED
    ${CMAKE_SOURCE_DIR}/altv-capi-server.cpp
)
target_compile_definitions(altv-capi-server PRIVATE
    ALT_SERVER_API
    CAPI_DLL
)
target_include_directories(altv-capi-server PRIVATE

)
set_target_properties(altv-capi-server PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server/lib
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server/lib
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server/bin
)
add_custom_command(TARGET altv-capi-server POST_BUILD # POST BUILD COMMANDS
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/server/altv-capi.h     ${PROJECT_BINARY_DIR}/altv-capi-server/include/server/altv-capi.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-server.h     ${PROJECT_BINARY_DIR}/altv-capi-server/include/altv-capi-server.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-predefines.h ${PROJECT_BINARY_DIR}/altv-capi-server/include/altv-capi-predefines.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-extra.h      ${PROJECT_BINARY_DIR}/altv-capi-server/include/altv-capi-extra.h
)

# Server static
add_library(altv-capi-server-static STATIC
    ${CMAKE_SOURCE_DIR}/altv-capi-server.cpp
)
target_compile_definitions(altv-capi-server-static PRIVATE
    ALT_SERVER_API
)
target_include_directories(altv-capi-server-static PRIVATE

)
set_target_properties(altv-capi-server-static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server-static/lib
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server-static/lib
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/altv-capi-server-static/bin
    POSITION_INDEPENDENT_CODE ON
)
add_custom_command(TARGET altv-capi-server POST_BUILD # POST BUILD COMMANDS
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/server/altv-capi.h     ${PROJECT_BINARY_DIR}/altv-capi-server-static/include/server/altv-capi.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-server.h     ${PROJECT_BINARY_DIR}/altv-capi-server-static/include/altv-capi-server.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-predefines.h ${PROJECT_BINARY_DIR}/altv-capi-server-static/include/altv-capi-predefines.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/altv-capi-extra.h      ${PROJECT_BINARY_DIR}/altv-capi-server-static/include/altv-capi-extra.h
)
