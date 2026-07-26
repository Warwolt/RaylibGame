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

# Tracy profiler
option(TRACY_STATIC "" OFF) # Tracy as DLL
option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)
option(TRACY_DELAYED_INIT "" ON)
option(TRACY_MANUAL_LIFETIME "" ON)
option(TRACY_NO_CALLSTACK "" ON)
add_subdirectory(lib/tracy)

# Source files
set(SRC
    src/core/debug/assert.cpp
    src/core/debug/assert.cpp
    src/core/debug/logging.cpp
    src/core/util.cpp
    src/core/util/time.cpp
    src/game/debug.cpp
    src/game/game.cpp
    src/game/resource.cpp
    src/game/scene/gameplay_scene.cpp
    src/game/scene/main_menu_scene.cpp
    src/game/scene/scene.cpp
    src/game/ui/context.cpp
    src/game/ui/element.cpp
    src/game/ui/style.cpp
    src/game/ui/tree.cpp
    src/game/ui/user_interface.cpp
    src/platform/input.cpp
    src/platform/win32.cpp
    src/platform/window.cpp
)
set(LIB
    raylib
    shlwapi
    dbghelp
    TracyClient
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
add_executable(Executable
    src/main.cpp
    src/library.cpp
    src/core/debug/assert.cpp
    src/core/debug/logging.cpp
    src/platform/win32.cpp
    src/core/util/time.cpp
)
target_include_directories(Executable PUBLIC src lib)
target_link_libraries(Executable ${LIB})
set_property(TARGET Executable PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Executable)
set_target_properties(Executable PROPERTIES OUTPUT_NAME "Game")

# Tests
add_executable(UnitTests
    $<TARGET_OBJECTS:Source>
    test/unit_tests/ui_element_tests.cpp
    test/unit_tests/ui_style_tests.cpp
    test/unit_tests/ui_tree_tests.cpp
    test/unit_tests/ui_user_interface_tests.cpp
    test/unit_tests/unit_test_main.cpp
)
target_include_directories(UnitTests PUBLIC src lib)
target_link_libraries(UnitTests PUBLIC ${LIB} gtest gmock)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UnitTests)

add_executable(SnapshotTests
    $<TARGET_OBJECTS:Source>
    test/snapshot_tests/snapshots_main.cpp
    test/snapshot_tests/snapshots.cpp
    test/snapshot_tests/ui_element_snapshot_tests.cpp
)
target_include_directories(SnapshotTests PUBLIC src lib)
target_link_libraries(SnapshotTests PUBLIC ${LIB} gtest gmock)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SnapshotTests)

# Set compilation options
SET(TARGETS Executable Library Source UnitTests SnapshotTests)
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

add_custom_command(TARGET Executable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib>
        $<TARGET_FILE_DIR:Executable>
    COMMENT "Copying Raylib DLL to build directory"
)

add_custom_command(TARGET Executable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:TracyClient>
        $<TARGET_FILE_DIR:Executable>
    COMMENT "Copying TracyClient DLL to build directory"
)
