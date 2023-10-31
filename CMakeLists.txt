cmake_minimum_required(VERSION 3.18)

project(Worldforge)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Meta data
set(DESCRIPTION "The Worldforge project.")
# Setting this at the top level prevents the libs from installing any artifacts.
# If you want the libs to install artifacts you can invoke cmake in the specific library directory or by setting NO_LIBS_INSTALL to "FALSE".
if (NOT DEFINED NO_LIBS_INSTALL)
    set(NO_LIBS_INSTALL "TRUE")
    message(STATUS "This is a mono repo build, which means that no artifacts from the libs will be installed. You can override this by setting NO_LIBS_INSTALL to 'FALSE', or by invoking cmake at each lib directory.")
endif ()

include(GNUInstallDirs)
include(FindPkgConfig)
include(CMakeDependentOption)

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
