## Package Status

| Bintray | Windows | Linux & macOS | 
|:--------:|:---------:|:-----------------:|
|[ ![Download](https://api.bintray.com/packages/bincrafters/public-conan/cmake_findboost_modular%3Abincrafters/images/download.svg) ](https://bintray.com/bincrafters/public-conan/cmake_findboost_modular%3Abincrafters/_latestVersion)|[![Build status](https://ci.appveyor.com/api/projects/status/github/bincrafters/conan-cmake_findboost_modular)](https://ci.appveyor.com/project/BinCrafters/conan-cmake_findboost_modular)|N/A|


## For Users: Use this package

### Basic setup

    $ conan install cmake_findboost_modular/1.69.0@bincrafters/stable

### Project setup

If you handle multiple dependencies in your project is better to add a *conanfile.txt*

    [requires]
    cmake_findboost_modular/1.69.0@bincrafters/stable

    [generators]
    txt

Complete the installation of requirements for your project running:

    $ mkdir build && cd build && conan install ..

Note: It is recommended that you run conan install from a build directory and not the root of the project directory.  This is because conan generates *conanbuildinfo* files specific to a single build configuration which by default comes from an autodetected default profile located in ~/.conan/profiles/default .  If you pass different build configuration options to conan install, it will generate different *conanbuildinfo* files.  Thus, they should not be added to the root of the project, nor committed to git.

### Usage in CMake

there are multiple varinats on how you can use `find_package`

#### CMake targets (modern CMake)

```
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
find_package(Boost COMPONENTS REQUIRED regex system date_time asio)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::regex Boost::system Boost::date_time Boost::asio)
```

#### CMake all-in-one target (modern CMake)

```
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
find_package(Boost COMPONENTS REQUIRED regex system date_time asio)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::Boost)
```

#### CMake variables (ancient CMake)
```
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
find_package(Boost COMPONENTS REQUIRED regex system date_time asio)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_REGEX_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_ASIO_LIBRARY})
```

#### Conan variables
```
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
find_package(Boost COMPONENTS REQUIRED regex system date_time asio)
target_include_directories(${PROJECT_NAME} PUBLIC ${CONAN_INCLUDE_DIRS_BOOST_REGEX} ${CONAN_INCLUDE_DIRS_BOOST_SYSTEM} ${CONAN_INCLUDE_DIRS_BOOST_DATE_TIME} ${CONAN_INCLUDE_DIRS_BOOST_ASIO})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS_BOOST_REGEX} ${CONAN_LIBS_BOOST_SYSTEM} ${CONAN_LIBS_BOOST_DATE_TIME} ${CONAN_LIBS_BOOST_ASIO})
```

#### Conan targets

```
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
find_package(Boost COMPONENTS REQUIRED regex system date_time asio)
target_link_libraries(${PROJECT_NAME} PUBLIC CONAN_PKG::boost_regex CONAN_PKG::boost_system CONAN_PKG::boost_date_time CONAN_PKG::boost_asio)
```

## For Packagers: Publish this Package

The example below shows the commands used to publish to bincrafters conan repository. To publish to your own conan respository (for example, after forking this git repository), you will need to change the commands below accordingly.

## Build and package

The following command both runs all the steps of the conan file, and publishes the package to the local system cache.  This includes downloading dependencies from "build_requires" and "requires" , and then running the build() method.

    $ conan create bincrafters/stable

## Add Remote

    $ conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"

## Upload

    $ conan upload cmake_findboost_modular/1.69.0@bincrafters/stable --all -r bincrafters

## License
[MIT](LICENSE)
