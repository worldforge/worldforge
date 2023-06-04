cmake_minimum_required(VERSION 2.8.12)
project(PackageTest CXX)

find_package(OGRE CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example ogre::ogre)
