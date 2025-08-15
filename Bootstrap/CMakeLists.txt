cmake_minimum_required(VERSION 3.16)
project(BootstrapExec LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

set(Bootstrap_SOURCES src/Bootstrap.cpp
        src/LuaManager.hpp
)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND Bootstrap_SOURCES src/Windows.cpp)
endif()

add_executable(Bootstrap ${Bootstrap_SOURCES})

target_include_directories(Bootstrap PRIVATE "${CMAKE_SOURCE_DIR}/Engine/include")
target_link_libraries(Bootstrap PRIVATE GameEngine)

target_compile_definitions(Bootstrap PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)

file(GLOB LUA_SRC
        "${CMAKE_SOURCE_DIR}/External/lua/src/*.c"
)
add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC "${CMAKE_SOURCE_DIR}/External/lua/src")

target_link_libraries(Bootstrap PRIVATE lua GameEngine)
target_include_directories(Bootstrap PRIVATE "${CMAKE_SOURCE_DIR}/External")

if(WIN32)
    target_compile_definitions(Bootstrap PRIVATE $<$<CONFIG:Release>:USE_WINMAIN_WRAPPER>)

    if(MSVC)
        target_compile_options(Bootstrap PRIVATE
                $<$<CONFIG:Release>:/O2 /DNDEBUG /GL>
        )
        target_link_options(Bootstrap PRIVATE
                $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
                $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /LTCG -s>
        )
    else()
        target_compile_options(Bootstrap PRIVATE
                $<$<CONFIG:Release>:-O2 -DNDEBUG -flto>
        )
        target_link_options(Bootstrap PRIVATE
                $<$<CONFIG:Debug>:-Wl,--subsystem,console>
                $<$<CONFIG:Release>:-Wl,--subsystem,windows -s>
        )
    endif()

    set(SDL3_DIR "${CMAKE_SOURCE_DIR}/External/SDL3")
    set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")

    target_include_directories(Bootstrap PRIVATE "${SDL3_DIR}/include")

    add_custom_command(TARGET Bootstrap POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL3_LIB_DIR}/SDL3.dll"
            "${SDL3_LIB_DIR}/SDL3_image.dll"
            $<TARGET_FILE_DIR:Bootstrap>
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Bootstrap>/assets"
    )

else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    pkg_check_modules(SDL3_IMAGE REQUIRED IMPORTED_TARGET sdl3-image)

    target_include_directories(Bootstrap PRIVATE
            ${SDL3_INCLUDE_DIRS}
            ${SDL3_IMAGE_INCLUDE_DIRS}
    )

    target_link_libraries(Bootstrap PRIVATE PkgConfig::SDL3 PkgConfig::SDL3_IMAGE)

    target_compile_options(Bootstrap PRIVATE
            $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native -flto>
    )

    add_custom_command(TARGET Bootstrap POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Bootstrap>/assets"
    )
endif()
