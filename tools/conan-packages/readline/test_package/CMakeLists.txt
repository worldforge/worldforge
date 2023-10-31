cmake_minimum_required(VERSION 2.8.12)
project(PackageTest C)

find_package(readline CONFIG REQUIRED)

add_executable(test_package test_package.c)
target_link_libraries(test_package readline::readline)
