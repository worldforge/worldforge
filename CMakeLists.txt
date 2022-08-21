cmake_minimum_required(VERSION 3.2)

project(squall CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
include(GNUInstallDirs)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    message(STATUS "Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else ()
    #TODO: check for correct libs so distros can package this.
    message(FATAL_ERROR "Building without Conan is currently not supported.")
endif (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")

enable_testing()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)