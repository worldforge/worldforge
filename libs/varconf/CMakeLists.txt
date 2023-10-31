cmake_minimum_required(VERSION 3.12)

project(varconf)
include(GNUInstallDirs)
include(FindPkgConfig)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)

# Version setup

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 3)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(ABI_CURRENT 9)
set(ABI_REVISION 0)
set(ABI_AGE 0)
math(EXPR SOVERSION ${ABI_CURRENT}-${ABI_AGE})
set(ABI_VERSION ${SOVERSION}.${ABI_AGE}.${ABI_REVISION})

# Set compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WF_WARNING_FLAGS /W3)
    add_definitions(-D_WIN32_WINNT=0x0601) #target Windows 7
else ()
    set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)
endif ()

include_directories("${PROJECT_SOURCE_DIR}/src")

# Meta data

set(DESCRIPTION "Configuration library for the Worldforge system.")


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

# Add test
enable_testing()

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
    target_compile_options(${TEST_NAME} PUBLIC "-w")
    target_link_libraries(${TEST_NAME} "${PROJECT_NAME}")
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    #We need to tell adjust the path so tests on windows can find the .dll files.
    SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin;$ENV{PATH}")

    add_dependencies(check ${TEST_NAME})
endmacro()

# Populate for pkg-config
set(REQUIRES "sigc++-3.0")

# Check for deps

find_package(sigc++-3 3.0 REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)

# pkg-config files
configure_file(tools/${PROJECT_NAME}.pc.in "${PROJECT_NAME}.pc" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Doxygen support, exports a "dox" target.

find_package(Doxygen)
set(DOXYGEN_INPUT src/varconf)
configure_file(docs/Doxyfile.in Doxyfile @ONLY)

if (DOXYGEN_FOUND)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docs)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT}
    )

endif (DOXYGEN_FOUND)

#add_custom_command(
#        OUTPUT ChangeLog
#        COMMAND ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} 8bd480b053190ffde2afe33af66f484953036f5a
#)
#add_custom_target(changelog DEPENDS ChangeLog)


# CMake config files
include(CMakePackageConfigHelpers)

install(EXPORT "${PROJECT_NAME}Targets"
        FILE "${PROJECT_NAME}Targets.cmake"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        )
configure_package_config_file(tools/Config.cmake.in
        ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
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
