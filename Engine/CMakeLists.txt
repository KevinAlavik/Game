cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

foreach(OUTPUTCONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_BINARY_DIR}/out/${OUTPUTCONFIG}")
endforeach()

set(SDL3_DIR "${CMAKE_SOURCE_DIR}/external/SDL3")
set(SDL3_INCLUDE_DIR "${SDL3_DIR}/include")
set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

set(ENGINE_SOURCES
    src/Renderer/Draw.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Window.cpp
    src/Renderer/TextureManager.cpp
    src/Math/Vector.cpp
    src/Core/Input.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(GameEngine STATIC ${ENGINE_SOURCES})

target_include_directories(GameEngine PUBLIC
    ${SDL3_INCLUDE_DIR}
    "${CMAKE_SOURCE_DIR}/Engine/include"
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_compile_definitions(GameEngine PUBLIC 
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

if(WIN32)
    target_compile_definitions(GameEngine PUBLIC $<$<CONFIG:Release>:USE_WINMAIN_WRAPPER>)
    target_compile_options(GameEngine PUBLIC
        $<$<CONFIG:Release>:/O2 /DNDEBUG /GL>
    )
else()
    target_compile_options(GameEngine PUBLIC
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native -flto>
    )
endif()

target_link_libraries(GameEngine PUBLIC
    "${SDL3_LIB_DIR}/SDL3.lib"
    "${SDL3_LIB_DIR}/SDL3_image.lib"
)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    target_include_directories(GameEngine PUBLIC ${OPENGL_INCLUDE_DIR})
    target_link_libraries(GameEngine PUBLIC ${OPENGL_gl_LIBRARY})
else()
    message(FATAL_ERROR "OpenGL not found")
endif()
