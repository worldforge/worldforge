cmake_minimum_required(VERSION 2.8.12)
project(PackageTest C)

find_package(Python3 3.10.13 EXACT COMPONENTS Development Interpreter REQUIRED)


add_executable(test_package py3/test_package.c)
target_link_libraries(test_package Python3::Python)
