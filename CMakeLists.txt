cmake_minimum_required(VERSION 3.18)

project(Worldforge)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)

# Meta data
set(DESCRIPTION "The Worldforge project.")
# Setting this at the top level prevents the libs from installing any artifacts.
# If you want the libs to install artifacts you can invoke cmake in the specific library directory or by setting NO_LIBS_INSTALL to "FALSE".
if (NOT DEFINED NO_LIBS_INSTALL)
    set(NO_LIBS_INSTALL "TRUE")
    message(STATUS "This is a mono repo build, which means that no artifacts from the libs will be installed. You can override this by setting NO_LIBS_INSTALL to 'FALSE', or by invoking cmake at each lib directory.")
endif ()
set(MONOREPO_BUILD "TRUE")

include(GNUInstallDirs)
include(FindPkgConfig)
include(CMakeDependentOption)

#If no VERSION is set externally, and we can't figure out the Git version, we'll use this version as fallback.
set(VERSION_FALLBACK "0.9.0-dev")

find_package(cppunit 1.8.0)
find_package(spdlog REQUIRED)

find_package(Git QUIET)

# Version setup. Figure out the version from 'git describe' if VERSION isn't set externally.
execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR} describe
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

if (NOT DEFINED SNAPCRAFT_COMMAND)
    #Allow for setting the path for the snapcraft command from outside, if setting PATH doesn't cut it.
    SET(SNAPCRAFT_COMMAND "snapcraft")
endif ()

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_WIN32_WINNT=0x0A00 WINVER=0x0A00) #target Windows 10
    # On Windows there are some headers that define "min" and "max" as preprocessor macros, which messes up compilation. This tells them to stop.
    add_compile_definitions("NOMINMAX")
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
        set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)
    endif ()
endif ()
MESSAGE(STATUS "Setting compiler warnings to '${WF_WARNING_FLAGS}'")
#Do this globally for the whole repo
add_compile_options(${WF_WARNING_FLAGS})

option(BUILD_TESTING "Should tests always be built; otherwise they will be built when the 'check' target is executed." OFF)
option(SKIP_EMBER "Skips building the Ember client." OFF)
option(SKIP_CYPHESIS "Skips building the Cyphesis server." OFF)
option(BUILD_METASERVER_SERVER "Includes the MetaServer server into the build. This isn't normally needed." OFF)

add_compile_definitions("PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")


if (APPLE)
    #On Mac we're having trouble with "iconv" not being linked properly, so we'll do it here.
    link_libraries(iconv)
endif ()

enable_testing()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -E Benchmark)
add_custom_target(benchmark COMMAND ${CMAKE_CTEST_COMMAND} -R Benchmark)
add_custom_target(dox)

#Since there are multiple instances where boost is required we specify this macro to keep the version in sync.
macro(wf_find_boost COMPONENTS)
    find_package(Boost
        1.85.0
        EXACT
        REQUIRED
        COMPONENTS ${COMPONENTS})
endmacro()

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)

# Packaging

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Worldforge")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "worldforge" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
)

include(CPack)
