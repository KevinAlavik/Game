cmake_minimum_required(VERSION 3.16)
project(GameExec LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

set(GAME_SOURCES src/Game.cpp)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND GAME_SOURCES src/Windows.cpp)
endif()

add_executable(Game ${GAME_SOURCES})

target_include_directories(Game PRIVATE "${CMAKE_SOURCE_DIR}/Engine/include")
target_link_libraries(Game PRIVATE GameEngine)

target_compile_definitions(Game PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

if(WIN32)
    target_compile_definitions(Game PRIVATE $<$<CONFIG:Release>:USE_WINMAIN_WRAPPER>)

    if(MSVC)
        target_compile_options(Game PRIVATE
            $<$<CONFIG:Release>:/O2 /DNDEBUG /GL>
        )
        target_link_options(Game PRIVATE
            $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
            $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /LTCG -s>
        )
    else()
        target_compile_options(Game PRIVATE
            $<$<CONFIG:Release>:-O2 -DNDEBUG -flto>
        )
        target_link_options(Game PRIVATE
            $<$<CONFIG:Debug>:-Wl,--subsystem,console>
            $<$<CONFIG:Release>:-Wl,--subsystem,windows -s>
        )
    endif()

    set(SDL3_DIR "${CMAKE_SOURCE_DIR}/External/SDL3")
    set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")

    target_include_directories(Game PRIVATE "${SDL3_DIR}/include")

    add_custom_command(TARGET Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL3_LIB_DIR}/SDL3.dll"
            "${SDL3_LIB_DIR}/SDL3_image.dll"
            $<TARGET_FILE_DIR:Game>
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Game>/assets"
    )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_custom_command(TARGET Game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                # THIS IS BAD
                "/usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll"
                "/usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll"
                "/usr/x86_64-w64-mingw32/bin/libstdc++-6.dll"
                $<TARGET_FILE_DIR:Game>
        )
    endif()

else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    pkg_check_modules(SDL3_IMAGE REQUIRED IMPORTED_TARGET sdl3-image)

    target_include_directories(Game PRIVATE
        ${SDL3_INCLUDE_DIRS}
        ${SDL3_IMAGE_INCLUDE_DIRS}
    )

    target_link_libraries(Game PRIVATE PkgConfig::SDL3 PkgConfig::SDL3_IMAGE)

    target_compile_options(Game PRIVATE
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native -flto>
    )

    add_custom_command(TARGET Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Game>/assets"
    )
endif()
