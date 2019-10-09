cmake_minimum_required(VERSION 3.2)

project(varconf)
include(GNUInstallDirs)
include(FindPkgConfig)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)

# Version setup

set(VARCONF_VERSION_MAJOR 1)
set(VARCONF_VERSION_MINOR 0)
set(VARCONF_VERSION_PATCH 3)

set(VERSION ${VARCONF_VERSION_MAJOR}.${VARCONF_VERSION_MINOR}.${VARCONF_VERSION_PATCH})
set(SUFFIX -${VARCONF_VERSION_MAJOR}.${VARCONF_VERSION_MINOR})

set(VARCONF_ABI_CURRENT 9)
set(VARCONF_ABI_REVISION 0)
set(VARCONF_ABI_AGE 0)
math(EXPR VARCONF_SOVERSION ${VARCONF_ABI_CURRENT}-${VARCONF_ABI_AGE})
set(VARCONF_ABI_VERSION ${VARCONF_SOVERSION}.${VARCONF_ABI_AGE}.${VARCONF_ABI_REVISION})

option(BUILD_TESTING "Should tests always be built; otherwise they will be built when the 'check' target is executed." OFF)

# Set compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WF_WARNING_FLAGS /W3)
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
            VERSION ${VARCONF_ABI_VERSION}
            SOVERSION ${VARCONF_SOVERSION}
            )
    target_compile_options(${_LIB_NAME} PRIVATE ${WF_WARNING_FLAGS})

    install(TARGETS ${_LIB_NAME}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

    target_link_libraries(${_LIB_NAME} ${WF_LIBRARIES})

endmacro()

# Add a "check" target, which builds and runs the tests.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

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
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME}${SUFFIX})
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)

    add_dependencies(check ${TEST_NAME})
endmacro()

# Check for deps

set(WF_LIBRARY_DIRS)
set(WF_INCLUDE_DIRS)
set(WF_LIBRARIES)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    MESSAGE("Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif ()

find_package(SigC++
        2.0
        REQUIRED)
list(APPEND WF_LIBRARIES ${SIGC++_LIBRARY})
list(APPEND WF_LIBRARY_DIRS ${SIGC++_LIBRARY_DIRS})
list(APPEND WF_INCLUDE_DIRS ${SIGC++_INCLUDE_DIR})

link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
# Populate for pkg-config
set(REQUIRES "sigc++-2.0")

add_subdirectory(src)
add_subdirectory(tests)

# pkg-config files
configure_file(tools/${PROJECT_NAME}.pc.in ${PROJECT_NAME}${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RPM spec files
#TODO: fix these
configure_file(tools/${PROJECT_NAME}.spec.in ${PROJECT_NAME}.spec @ONLY)
#TODO: fix these
configure_file(tools/mingw32-${PROJECT_NAME}.spec.in mingw32-${PROJECT_NAME}.spec @ONLY)

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

    add_custom_target(dox DEPENDS ${DOXYGEN_OUTPUT})

endif (DOXYGEN_FOUND)

add_custom_command(
        OUTPUT ChangeLog
        COMMAND ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} 8bd480b053190ffde2afe33af66f484953036f5a
)
add_custom_target(changelog DEPENDS ChangeLog)


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


# Packaging (for source tarballs

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Worldforge")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${VARCONF_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VARCONF_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VARCONF_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)