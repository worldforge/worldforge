project(test_package)
cmake_minimum_required(VERSION 2.8.11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

if(${CONAN_VARIANT} STREQUAL "CONAN_TARGETS")
    conan_basic_setup(TARGETS)
else()
    conan_basic_setup()
endif()

add_executable(${PROJECT_NAME} test_package.cpp)

find_package(Boost COMPONENTS REQUIRED regex system date_time asio)

if(Boost_FOUND)
    if(${BUILD_VARIANT} STREQUAL "FIND_PACKAGE")
        target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_REGEX_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_ASIO_LIBRARY})
    endif()
    if(${BUILD_VARIANT} STREQUAL "FIND_PACKAGE_TARGETS")
        target_link_libraries(${PROJECT_NAME} PUBLIC Boost::regex Boost::system Boost::date_time Boost::asio)
    endif()
else()
    message(FATAL_ERROR "boost wasn't found!")
endif()
