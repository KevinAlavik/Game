cmake_minimum_required(VERSION 3.16)
project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

foreach(OUTPUTCONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_BINARY_DIR}/out/${OUTPUTCONFIG}")
endforeach()

add_subdirectory(Engine)
add_subdirectory(Game)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "assets")

if(WIN32)
    set(SDL3_DIR "${CMAKE_SOURCE_DIR}/External/SDL3")
    set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")

    install(FILES
        "${SDL3_LIB_DIR}/SDL3.dll"
        "${SDL3_LIB_DIR}/SDL3_image.dll"
        DESTINATION "."
    )
endif()
