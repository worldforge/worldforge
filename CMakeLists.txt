cmake_minimum_required(VERSION 3.2)

project(squall CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
include(GNUInstallDirs)

# Version setup

set(SQUALL_VERSION_MAJOR 0)
set(SQUALL_VERSION_MINOR 1)
set(SQUALL_VERSION_PATCH 0)

set(VERSION ${SQUALL_VERSION_MAJOR}.${SQUALL_VERSION_MINOR}.${SQUALL_VERSION_PATCH})
set(SUFFIX -${SQUALL_VERSION_MAJOR}.${SQUALL_VERSION_MINOR})

set(SQUALL_ABI_CURRENT 0)
set(SQUALL_ABI_REVISION 0)
set(SQUALL_ABI_AGE 0)
math(EXPR SQUALL_SOVERSION ${SQUALL_ABI_CURRENT}-${SQUALL_ABI_AGE})
set(SQUALL_ABI_VERSION ${SQUALL_SOVERSION}.${SQUALL_ABI_AGE}.${SQUALL_ABI_REVISION})


if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    message(STATUS "Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    #TODO: check for correct libs so distros can package this.
    message(FATAL_ERROR "Building without Conan is currently not supported.")
endif (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")


# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}})
    #add_library(${_LIB_NAME}_object OBJECT ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${SQUALL_ABI_VERSION}
            SOVERSION ${SQUALL_SOVERSION}
            )

    install(TARGETS ${_LIB_NAME}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

    target_link_libraries(${_LIB_NAME} PUBLIC ${WF_LIBRARIES})
    target_compile_options(${_LIB_NAME} PRIVATE ${WF_WARNING_FLAGS})

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
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)

    if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        target_link_libraries(${TEST_NAME} PUBLIC
                ${CONAN_LIBS})
    endif ()

endmacro()

enable_testing()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)


# CMake config files
include(CMakePackageConfigHelpers)

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
