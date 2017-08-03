cmake_minimum_required(VERSION 3.2)

project(wfmath)
set(CMAKE_CXX_STANDARD 14)
include(GNUInstallDirs)
include(FindPkgConfig)

# Version setup

set(WFMATH_VERSION_MAJOR 1)
set(WFMATH_VERSION_MINOR 0)
set(WFMATH_VERSION_PATCH 3)

set(VERSION ${WFMATH_VERSION_MAJOR}.${WFMATH_VERSION_MINOR}.${WFMATH_VERSION_PATCH})
set(SUFFIX -${WFMATH_VERSION_MAJOR}.${WFMATH_VERSION_MINOR})

set(WFMATH_ABI_CURRENT 1)
set(WFMATH_ABI_REVISION 0)
set(WFMATH_ABI_AGE 0)
math(EXPR WFMATH_SOVERSION ${WFMATH_ABI_CURRENT}-${WFMATH_ABI_AGE})
set(WFMATH_ABI_VERSION ${WFMATH_SOVERSION}.${WFMATH_ABI_AGE}.${WFMATH_ABI_REVISION})

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories("${PROJECT_SOURCE_DIR}")

# Meta data

set(DESCRIPTION "A math library for the Worldforge system.")



# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} SHARED ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${WFMATH_ABI_VERSION}
            SOVERSION ${WFMATH_SOVERSION}
            )

    install(TARGETS ${_LIB_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

    set(PKG_CONFIG_LIBS "-l${_LIB_NAME} ${PKG_CONFIG_LIBS}")

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


# We only need Atlas for tests
pkg_check_modules(WF atlascpp-0.7>=0.7)

if (WF_FOUND)
    wf_add_test(atlas/atlas_0_7_test.cpp)
    target_link_libraries(atlas_0_7_test ${WF_LDFLAGS})
    target_include_directories(atlas_0_7_test PUBLIC ${WF_INCLUDE_DIRS})
endif (WF_FOUND)

# Define source files for libraries

set(SOURCE_FILES
        wfmath/axisbox.cpp
        wfmath/ball.cpp
        wfmath/const.cpp
        wfmath/int_to_string.cpp
        wfmath/intersect.cpp
        wfmath/line.cpp
        wfmath/point.cpp
        wfmath/polygon.cpp
        wfmath/polygon_intersect.cpp
        wfmath/probability.cpp
        wfmath/quaternion.cpp
        wfmath/randgen.cpp
        wfmath/rotbox.cpp
        wfmath/rotmatrix.cpp
        wfmath/segment.cpp
        wfmath/stream.cpp
        wfmath/timestamp.cpp
        wfmath/vector.cpp)

set(HEADER_FILES
        wfmath/atlasconv.h
        wfmath/axisbox.h
        wfmath/axisbox_funcs.h
        wfmath/ball.h
        wfmath/ball_funcs.h
        wfmath/basis.h
        wfmath/const.h
        wfmath/error.h
        wfmath/general_test.h
        wfmath/int_to_string.h
        wfmath/intersect.h
        wfmath/intersect_decls.h
        wfmath/line.h
        wfmath/line_funcs.h
        wfmath/MersenneTwister.h
        wfmath/miniball.h
        wfmath/miniball_funcs.h
        wfmath/point.h
        wfmath/point_funcs.h
        wfmath/polygon.h
        wfmath/polygon_funcs.h
        wfmath/polygon_intersect.h
        wfmath/probability.h
        wfmath/quaternion.h
        wfmath/randgen.h
        wfmath/rotbox.h
        wfmath/rotbox_funcs.h
        wfmath/rotmatrix.h
        wfmath/rotmatrix_funcs.h
        wfmath/segment.h
        wfmath/segment_funcs.h
        wfmath/shuffle.h
        wfmath/stream.h
        wfmath/timestamp.h
        wfmath/vector.h
        wfmath/vector_funcs.h
        wfmath/wfmath.h
        wfmath/wrapped_array.h
        wfmath/zero.h)

wf_add_library(${PROJECT_NAME}${SUFFIX} SOURCE_FILES HEADER_FILES)


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

wf_add_test(wfmath/ball_test.cpp)
wf_add_test(wfmath/const_test.cpp)
wf_add_test(wfmath/intstring_test.cpp)
wf_add_test(wfmath/line_test.cpp)
wf_add_test(wfmath/point_test.cpp)
wf_add_test(wfmath/polygon_test.cpp)
wf_add_test(wfmath/probability_test.cpp)
wf_add_test(wfmath/quaternion_test.cpp)
wf_add_test(wfmath/randgen_test.cpp)
wf_add_test(wfmath/rotmatrix_test.cpp)
wf_add_test(wfmath/shape_test.cpp)
wf_add_test(wfmath/timestamp_test.cpp)
wf_add_test(wfmath/vector_test.cpp)


# Doxygen support, exports a "docs" target.

find_package(Doxygen)
configure_file(doc/Doxyfile.in Doxyfile @ONLY)

if (DOXYGEN_FOUND)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_OUTPUT doc)

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
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${WFMATH_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${WFMATH_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${WFMATH_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)