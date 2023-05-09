cmake_minimum_required(VERSION 2.8.12)
project(PackageTest CXX)

find_package(freeimage CONFIG REQUIRED)

add_executable(example test_package.cpp)
target_link_libraries(example freeimage::freeimage)
configure_file(test.png test.png COPYONLY)
