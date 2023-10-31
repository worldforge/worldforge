cmake_minimum_required(VERSION 3.18)

project(Worldforge)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Meta data
set(DESCRIPTION "The Worldforge project.")

include(GNUInstallDirs)
include(FindPkgConfig)
include(CMakeDependentOption)

# Version setup. Figure out the version from 'git describe' if VERSION isn't set externally.
execute_process(COMMAND git -C ${PROJECT_SOURCE_DIR} describe
        OUTPUT_VARIABLE GIT_REPO_VERSION)
string(REGEX REPLACE "\n$" "" GIT_REPO_VERSION "${GIT_REPO_VERSION}")
if (NOT VERSION)
    if (NOT GIT_REPO_VERSION)
        set(VERSION "${VERSION_FALLBACK}")
    else ()
        set(VERSION "${GIT_REPO_VERSION}")
    endif ()
endif ()

if (NOT VERSION_PACKAGE)
    message(STATUS "Setting VERSION_PACKAGE to ${VERSION}. You can override this by setting the 'VERSION_PACKAGE' variable externally.")
    set(VERSION_PACKAGE "${VERSION}")
endif ()

message(STATUS "Building version ${VERSION}")

#Split a version number like "1.2.3" into a list to extract parts.
string(REPLACE "." ";" VERSION_LIST ${VERSION})
list(GET VERSION_LIST 0 EMBER_VERSION_MAJOR)
list(GET VERSION_LIST 1 EMBER_VERSION_MINOR)
list(GET VERSION_LIST 2 EMBER_VERSION_PATCH)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

# Set compiler flags, but only if not running in a CI server. CI environments would normally set the environment variable CI.
if (DEFINED ENV{CI})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(WF_WARNING_FLAGS "")
    else ()
        #Turn of diagnostics. Perhaps a bit too harsh?
        set(WF_WARNING_FLAGS "-w")
    endif ()
else ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(WF_WARNING_FLAGS /W3)
    else ()
        set(WF_WARNING_FLAGS -Wall -Winit-self -Wwrite-strings -Wundef -Wno-unused-parameter -Wno-missing-field-initializers -Wno-long-long)
    endif ()
endif ()
MESSAGE(STATUS "Setting compiler warnings to '${WF_WARNING_FLAGS}'")

option(BUILD_TESTING "Should tests always be built; otherwise they will be built when the 'check' target is executed." OFF)


add_compile_definitions("PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

# On Windows there are some headers that define "min" and "max" as preprocessor macros, which messes up compilation. This tells them to stop.
add_compile_definitions("NOMINMAX")

if (APPLE)
    #On Mac we're having trouble with "iconv" not being linked properly, so we'll do it here.
    link_libraries(iconv)
endif ()

enable_testing()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -E Benchmark)
add_custom_target(benchmark COMMAND ${CMAKE_CTEST_COMMAND} -R Benchmark)

add_custom_target(dox)


add_subdirectory(libs)
add_subdirectory(apps)
