cmake_minimum_required(VERSION 3.2)

project(Mercator)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
include(GNUInstallDirs)
include(FindPkgConfig)

# Version setup

set(MERCATOR_VERSION_MAJOR 0)
set(MERCATOR_VERSION_MINOR 4)
set(MERCATOR_VERSION_PATCH 0)

set(VERSION ${MERCATOR_VERSION_MAJOR}.${MERCATOR_VERSION_MINOR}.${MERCATOR_VERSION_PATCH})
set(SUFFIX -${MERCATOR_VERSION_MAJOR}.${MERCATOR_VERSION_MINOR})

set(MERCATOR_ABI_CURRENT 0)
set(MERCATOR_ABI_REVISION 0)
set(MERCATOR_ABI_AGE 0)
math(EXPR MERCATOR_SOVERSION ${MERCATOR_ABI_CURRENT}-${MERCATOR_ABI_AGE})
set(MERCATOR_ABI_VERSION ${MERCATOR_SOVERSION}.${MERCATOR_ABI_AGE}.${MERCATOR_ABI_REVISION})

# Set compiler flags
set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)

include_directories("${PROJECT_SOURCE_DIR}/src")

# Meta data

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
set(DESCRIPTION "A config handling library for the Worldforge system.")

# Check for libraries
pkg_check_modules(WF REQUIRED wfmath-1.0>=1.0.0)

link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
# Populate for pkg-config
set(REQUIRES "wfmath-1.0")

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} SHARED ${${_SOURCE_FILES_VAR}})
    add_library(${_LIB_NAME}_STATIC STATIC ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${MERCATOR_ABI_VERSION}
            SOVERSION ${MERCATOR_SOVERSION}
            )

    install(TARGETS ${_LIB_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

    target_link_libraries(${_LIB_NAME} ${WF_LIBRARIES})
    target_link_libraries(${_LIB_NAME}_STATIC ${WF_LIBRARIES})
    target_compile_options(${_LIB_NAME} PRIVATE ${WF_WARNING_FLAGS})

endmacro()

# pkg-config files
configure_file(tools/${PROJECT_NAME_LOWER}.pc.in ${PROJECT_NAME_LOWER}${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RPM spec files
#TODO: fix these
configure_file(tools/${PROJECT_NAME_LOWER}.spec.in ${PROJECT_NAME_LOWER}.spec @ONLY)
#TODO: fix these
configure_file(tools/mingw32-${PROJECT_NAME_LOWER}.spec.in mingw32-${PROJECT_NAME_LOWER}.spec @ONLY)

# Add test
enable_testing()

# Add a "check" target, which builds and runs the tests.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

#Macro for adding a test. The test name will be extracted from the name of the first submitted file.
#Additional files can be submitted as varargs.
macro(wf_add_test TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME_LOWER}${SUFFIX}_STATIC)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})
endmacro()

add_subdirectory(src)
add_subdirectory(tests)

# Doxygen support, exports a "docs" target.

find_package(Doxygen)
set(DOXYGEN_INPUT src/Mercator)
configure_file(docs/Doxyfile.in Doxyfile @ONLY)

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
set(CPACK_PACKAGE_VERSION_MAJOR "${MERCATOR_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MERCATOR_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MERCATOR_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)



