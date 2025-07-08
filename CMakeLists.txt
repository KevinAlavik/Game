cmake_minimum_required(VERSION 3.16)
project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# SDL3
set(SDL3_DIR "${CMAKE_SOURCE_DIR}/external/SDL3")
set(SDL3_INCLUDE_DIR "${SDL3_DIR}/include")
set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")

# ImGui
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

add_executable(Game
    src/Game.cpp
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

target_include_directories(Game PRIVATE
    ${SDL3_INCLUDE_DIR}
    "${CMAKE_SOURCE_DIR}/include"
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_compile_definitions(Game PRIVATE 
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

target_link_libraries(Game 
    "${SDL3_LIB_DIR}/SDL3.lib"
    "${SDL3_LIB_DIR}/SDL3_image.lib"
)

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    target_include_directories(Game PRIVATE ${OPENGL_INCLUDE_DIR})
    target_link_libraries(Game ${OPENGL_gl_LIBRARY})
else()
    message(FATAL_ERROR "OpenGL not found")
endif()

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_LIB_DIR}/SDL3.dll"
        "${SDL3_LIB_DIR}/SDL3_image.dll"
        $<TARGET_FILE_DIR:Game>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Game>/assets"
)
