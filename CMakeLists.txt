cmake_minimum_required(VERSION 3.16)
project(Bootstrap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

# Output directories per config
foreach(OUTPUTCONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_BINARY_DIR}/out/${OUTPUTCONFIG}")
endforeach()

# External and Engine directories
add_subdirectory(Engine)
add_subdirectory(Bootstrap)

# Install assets
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

# --- Embedded Lua ---
set(SANDBOX_DIR "${CMAKE_SOURCE_DIR}/Sandbox")
set(GAME_LUA "${SANDBOX_DIR}/game.lua")
set(GAME_LUA_HPP "${CMAKE_BINARY_DIR}/generated/game_lua.hpp")

add_custom_command(
        OUTPUT ${GAME_LUA_HPP}
        COMMAND ${CMAKE_COMMAND} -DINPUT=${GAME_LUA} -DOUTPUT=${GAME_LUA_HPP} -P ${CMAKE_SOURCE_DIR}/cmake/GenerateLuaCpp.cmake
        DEPENDS ${GAME_LUA}
        COMMENT "Generating C++ header from Lua script"
)

add_custom_target(GenerateLuaHeader ALL DEPENDS ${GAME_LUA_HPP})

target_include_directories(Bootstrap PRIVATE "${CMAKE_BINARY_DIR}/generated")
add_dependencies(Bootstrap GenerateLuaHeader)
