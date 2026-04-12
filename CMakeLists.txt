cmake_minimum_required(VERSION 3.16.0)
project(RaylibGame)
enable_language(CXX)

option(WARNINGS_AS_ERRORS "Compile with warnings as errors" OFF)

# Add Raylib
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(lib/raylib)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Google Test
add_subdirectory(lib/googletest)

# Windows workaround for Google Test
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
endif()

# Source files
set(SRC
    src/core/debug/logging.cpp
    src/core/util.cpp
    src/game/game.cpp
    src/game/resource.cpp
    src/game/scene/gameplay.cpp
    src/game/scene/main_menu.cpp
    src/game/scene/scene.cpp
    src/game/ui.cpp
    src/platform/window.cpp
)
set(LIB
    raylib
    shlwapi
)

set(INC .)

# Source library
add_library(Source OBJECT ${SRC})
target_include_directories(Source PUBLIC src lib)
target_link_libraries(Source PUBLIC ${LIB})

# Hot reload DLL
add_library(Library SHARED $<TARGET_OBJECTS:Source>)
target_include_directories(Library PUBLIC src lib)
target_link_libraries(Library ${LIB})
set_target_properties(Library PROPERTIES OUTPUT_NAME "GameLib")

# Executable
add_executable(Executable src/main.cpp)
target_include_directories(Executable PUBLIC src lib)
target_link_libraries(Executable ${LIB})
set_property(TARGET Executable PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Executable)
set_target_properties(Executable PROPERTIES OUTPUT_NAME "Game")

# Tests
add_executable(Tests
    $<TARGET_OBJECTS:Source>
    test/snapshots.cpp
    test/ui_layout_tests.cpp
    test/ui_interaction_tests.cpp
    test/main.cpp
)
target_include_directories(Tests PUBLIC src lib)
target_link_libraries(Tests PUBLIC ${LIB} gtest gmock)


# Set compilation options
SET(TARGETS Executable Library Source Tests)
foreach(TARGET IN ITEMS ${TARGETS})
    target_include_directories(${TARGET} PRIVATE ${INC})
    target_compile_options(${TARGET} PUBLIC
        /W4
        /w44062 # enable warning on unhandled switch case
        /wd4996 # ignore MSVC insisting on _s functions
        /wd4244 # ignore conversion from 'type1' to 'type2', possible loss of data
        /wd4838 # ignore conversion from 'type_1' to 'type_2' requires a narrowing conversion
    )
    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 23)
    if (WARNINGS_AS_ERRORS)
        target_compile_options(${TARGET} PUBLIC /WX)
    endif()
endforeach()

# Copy raylib DLL to executable directory
add_custom_command(TARGET Executable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib>
        $<TARGET_FILE_DIR:Executable>
    COMMENT "Copy raylib DLL to executable directory"
)
