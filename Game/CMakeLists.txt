set(SDL3_DIR "${CMAKE_SOURCE_DIR}/External/SDL3")
set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")

set(GAME_SOURCES 
    "src/Game.cpp"
)

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND GAME_SOURCES "src/Windows.cpp")
    endif()
endif()

add_executable(Game ${GAME_SOURCES})

target_include_directories(Game PRIVATE
    "${SDL3_DIR}/include"
    "${CMAKE_SOURCE_DIR}/Engine/include"
)

target_link_libraries(Game PRIVATE GameEngine)

target_compile_definitions(Game PRIVATE 
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

if(WIN32)
    target_compile_definitions(Game PRIVATE $<$<CONFIG:Release>:USE_WINMAIN_WRAPPER>)
    target_compile_options(Game PRIVATE
        $<$<CONFIG:Release>:/O2 /DNDEBUG /GL>
    )
    target_link_options(Game PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /LTCG>
        $<$<CONFIG:Release>:-s>
    )
else()
    target_compile_options(Game PRIVATE
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native -flto>
    )
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

install(TARGETS Game DESTINATION .)
install(FILES
    "${SDL3_LIB_DIR}/SDL3.dll"
    "${SDL3_LIB_DIR}/SDL3_image.dll"
    DESTINATION .
)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets" DESTINATION .)
