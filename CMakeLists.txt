cmake_minimum_required(VERSION 3.2)

project(varconf)
include(GNUInstallDirs)
include(FindPkgConfig)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

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

# Set compiler flags
set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)

include_directories("${PROJECT_SOURCE_DIR}")

# Meta data

set(DESCRIPTION "Configuration library for the Worldforge system.")



# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} SHARED ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${VARCONF_ABI_VERSION}
            SOVERSION ${VARCONF_SOVERSION}
            )

    install(TARGETS ${_LIB_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

    set(PKG_CONFIG_LIBS "-l${_LIB_NAME} ${PKG_CONFIG_LIBS}")

    target_link_libraries(${_LIB_NAME} ${WF_LIBRARIES})

endmacro()

# Add a "check" target, which builds and runs the tests.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

#Macro for adding a test. The test name will be extracted from the name of the first submitted file.
#Additional files can be submitted as varargs.
macro(wf_add_test TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME}${SUFFIX})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})
endmacro()


# Check for deps
pkg_check_modules(WF REQUIRED sigc++-2.0>=2.0.0)

link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
# Populate for pkg-config
set(REQUIRES "sigc++-2.0")

# Define source files for libraries

set(SOURCE_FILES
        varconf/config.cpp
        varconf/dynbase.cpp
        varconf/dyncmp.cpp
        varconf/dyntypes.cpp
        varconf/parse_error.cpp
        varconf/varconf.h
        varconf/variable.cpp)

set(HEADER_FILES
        varconf/config.h
        varconf/dynbase.h
        varconf/dyncmp.h
        varconf/dyntypes.h
        varconf/dynvar.h
        varconf/parse_error.h
        varconf/varconf.h
        varconf/varconf_defs.h
        varconf/variable.h)

wf_add_library(${PROJECT_NAME}${SUFFIX} SOURCE_FILES HEADER_FILES)
target_compile_options("${PROJECT_NAME}${SUFFIX}" PRIVATE ${WF_WARNING_FLAGS})

# pkg-config files
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RPM spec files
#TODO: fix these
configure_file(${PROJECT_NAME}.spec.in ${PROJECT_NAME}.spec @ONLY)
#TODO: fix these
configure_file(mingw32-${PROJECT_NAME}.spec.in mingw32-${PROJECT_NAME}.spec @ONLY)

# Add test
enable_testing()



wf_add_test(tests/conftest.cpp)
add_definitions(-DSRCDIR="${PROJECT_SOURCE_DIR}/tests")

# Doxygen support, exports a "docs" target.

find_package(Doxygen)
set(DOXYGEN_INPUT varconf)
configure_file(Doxyfile.in Doxyfile @ONLY)

if (DOXYGEN_FOUND)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doc)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT}
    )

    add_custom_target(docs DEPENDS ${DOXYGEN_OUTPUT})

endif (DOXYGEN_FOUND)

add_custom_command(
        OUTPUT ChangeLog
        COMMAND ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} 8bd480b053190ffde2afe33af66f484953036f5a
)
add_custom_target(changelog DEPENDS ChangeLog)


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