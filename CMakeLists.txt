cmake_minimum_required(VERSION 3.12)

project(squall CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
include(GNUInstallDirs)

# Version setup

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(ABI_CURRENT 0)
set(ABI_REVISION 0)
set(ABI_AGE 0)
math(EXPR SOVERSION ${ABI_CURRENT}-${ABI_AGE})
set(ABI_VERSION ${SOVERSION}.${ABI_AGE}.${ABI_REVISION})

if (EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
    message(STATUS "Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
    if (${CONAN_CMAKE_POSITION_INDEPENDENT_CODE})
        message(STATUS "Enabling position independent code")
        set(CMAKE_POSITION_INDEPENDENT_CODE ${CONAN_CMAKE_POSITION_INDEPENDENT_CODE})
    endif ()
endif ()

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#link_libraries(Threads::Threads)

find_package(Catch2 REQUIRED)

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${ABI_VERSION}
            SOVERSION ${SOVERSION}
            )
    target_compile_options(${_LIB_NAME} PUBLIC ${WF_WARNING_FLAGS})
    target_include_directories(${_LIB_NAME}
            PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
            )
    install(TARGETS ${_LIB_NAME}
            EXPORT "${PROJECT_NAME}Targets"
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )
    #We would want to use the FILE_SET feature, but that requires CMake 3.23, which prevents us from currently building
    # Snap packages (as of 2023-02-12). Instead we'll copy each include file.
    #When distros catch up to more recent CMake versions we can use FILE_SET instead.
    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${dir})
    endforeach ()

endmacro()

#Macro for adding a test. The test name will be extracted from the name of the first submitted file.
#Additional files can be submitted as varargs.
macro(wf_add_test TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # If BUILD_TESTING is defined we'll build the test no matter what. This makes it work better on Windows.
    if (BUILD_TESTING)
        add_executable(${TEST_NAME} ${TEST_FILE} ${ARGN})
    else (BUILD_TESTING)
        add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    endif (BUILD_TESTING)
    target_link_libraries(${TEST_NAME} Catch2::Catch2WithMain)
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    #We need to tell adjust the path so tests on windows can find the .dll files.
    SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin;$ENV{PATH}")
endmacro()

enable_testing()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)


# CMake config files
include(CMakePackageConfigHelpers)

install(EXPORT "${PROJECT_NAME}Targets"
        FILE "${PROJECT_NAME}Targets.cmake"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        )
configure_package_config_file(tools/Config.cmake.in ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR)
write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${VERSION}
        COMPATIBILITY ExactVersion)
install(FILES
        ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
        ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})