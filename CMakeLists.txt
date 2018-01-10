cmake_minimum_required(VERSION 3.2)

project(Mercator)
set(CMAKE_CXX_STANDARD 14)
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories("${PROJECT_SOURCE_DIR}")

# Meta data

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
set(DESCRIPTION "A config handling library for the Worldforge system.")

# Check for libraries
pkg_check_modules(WF REQUIRED wfmath-1.0>=1.0.0)

link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
# Populate for pkg-config
set(REQUIRES "wfmath-1.0")

# Define source files for libraries

set(SOURCE_FILES
        Mercator/Area.cpp
        Mercator/AreaShader.cpp
        Mercator/BasePoint.cpp
        Mercator/Buffer.cpp
        Mercator/Buffer_impl.h
        Mercator/DepthShader.cpp
        Mercator/Effector.cpp
        Mercator/FillShader.cpp
        Mercator/Forest.cpp
        Mercator/GrassShader.cpp
        Mercator/HeightMap.cpp
        Mercator/Intersect.cpp
        Mercator/Matrix.cpp
        Mercator/Mercator.cpp
        Mercator/Plant.cpp
        Mercator/Segment.cpp
        Mercator/Shader.cpp
        Mercator/ShaderFactory.cpp
        Mercator/ShaderFactory_impl.h
        Mercator/Surface.cpp
        Mercator/Terrain.cpp
        Mercator/TerrainMod.cpp
        Mercator/TerrainMod_impl.h
        Mercator/ThresholdShader.cpp
        Mercator/TileShader.cpp
        Mercator/iround.h)

set(HEADER_FILES
        Mercator/Area.h
        Mercator/AreaShader.h
        Mercator/BasePoint.h
        Mercator/Buffer.h
        Mercator/DepthShader.h
        Mercator/Effector.h
        Mercator/FillShader.h
        Mercator/Forest.h
        Mercator/GrassShader.h
        Mercator/HeightMap.h
        Mercator/Intersect.h
        Mercator/iround.h
        Mercator/Matrix.h
        Mercator/Mercator.h
        Mercator/Plant.h
        Mercator/RandCache.h
        Mercator/Segment.h
        Mercator/Shader.h
        Mercator/ShaderFactory.h
        Mercator/ShaderFactory_impl.h
        Mercator/Surface.h
        Mercator/Terrain.h
        Mercator/TerrainMod.h
        Mercator/TerrainMod_impl.h
        Mercator/ThresholdShader.h
        Mercator/TileShader.h)


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

    set(PKG_CONFIG_LIBS "-l${_LIB_NAME} ${PKG_CONFIG_LIBS}")

    target_link_libraries(${_LIB_NAME} ${WF_LIBRARIES})
    target_link_libraries(${_LIB_NAME}_STATIC ${WF_LIBRARIES})

endmacro()

wf_add_library(mercator${SUFFIX} SOURCE_FILES HEADER_FILES)


# pkg-config files
configure_file(${PROJECT_NAME_LOWER}.pc.in ${PROJECT_NAME_LOWER}${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RPM spec files
#TODO: fix these
configure_file(${PROJECT_NAME_LOWER}.spec.in ${PROJECT_NAME_LOWER}.spec @ONLY)
#TODO: fix these
configure_file(mingw32-${PROJECT_NAME_LOWER}.spec.in mingw32-${PROJECT_NAME_LOWER}.spec @ONLY)

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


wf_add_test(tests/main.cpp)
wf_add_test(tests/Terraintest.cpp)
wf_add_test(tests/Shadertest.cpp)
wf_add_test(tests/TerrainModtest.cpp)
wf_add_test(tests/Intersecttest.cpp)
wf_add_test(tests/testPhys.cpp)
wf_add_test(tests/timeSeg.cpp tests/util_timer.cpp tests/util_timer.h)
wf_add_test(tests/Planttest.cpp)
wf_add_test(tests/Foresttest.cpp)
wf_add_test(tests/testQRNG.cpp)
wf_add_test(tests/Areatest.cpp)
wf_add_test(tests/TileShadertest.cpp)
wf_add_test(tests/ShaderFactorytest.cpp)
wf_add_test(tests/testWFMath.cpp)
wf_add_test(tests/Buffertest.cpp)
wf_add_test(tests/Segmenttest.cpp)
wf_add_test(tests/AreaShadertest.cpp)
wf_add_test(tests/BasePointtest.cpp)
wf_add_test(tests/DepthShadertest.cpp)
wf_add_test(tests/FillShadertest.cpp)
wf_add_test(tests/GrassShadertest.cpp)
wf_add_test(tests/ThresholdShadertest.cpp)
wf_add_test(tests/Matrixtest.cpp)
wf_add_test(tests/TerrainaddAreatest.cpp)
wf_add_test(tests/Segmentperf.cpp)

# Doxygen support, exports a "docs" target.

find_package(Doxygen)
set(DOXYGEN_INPUT Mercator)
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



