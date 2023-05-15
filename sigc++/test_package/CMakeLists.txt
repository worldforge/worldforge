cmake_minimum_required(VERSION 2.8.12)
project(PackageTest CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(sigc++ CONFIG REQUIRED)

add_executable(test_bind test_bind.cc testutilities.cc)
target_link_libraries(test_bind sigc++::sigc++)
