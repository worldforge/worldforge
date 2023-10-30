cmake_minimum_required(VERSION 2.8.12)
project(PackageTest CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package(squall CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example squall::squall)
