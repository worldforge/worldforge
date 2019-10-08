cmake_minimum_required(VERSION 3.2)
project(Atlas)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
include(GNUInstallDirs)

# Version setup

set(ATLAS_VERSION_MAJOR 0)
set(ATLAS_VERSION_MINOR 7)
set(ATLAS_VERSION_PATCH 0)

set(VERSION ${ATLAS_VERSION_MAJOR}.${ATLAS_VERSION_MINOR}.${ATLAS_VERSION_PATCH})
set(SUFFIX -${ATLAS_VERSION_MAJOR}.${ATLAS_VERSION_MINOR})

set(ATLAS_ABI_CURRENT 0)
set(ATLAS_ABI_REVISION 0)
set(ATLAS_ABI_AGE 0)
math(EXPR ATLAS_SOVERSION ${ATLAS_ABI_CURRENT}-${ATLAS_ABI_AGE})
set(ATLAS_ABI_VERSION ${ATLAS_SOVERSION}.${ATLAS_ABI_AGE}.${ATLAS_ABI_REVISION})

# Set compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WF_WARNING_FLAGS /Wall)
else ()
    set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)
endif ()

# Meta data

set(DESCRIPTION "Networking protocol for the Worldforge system.")

# Check for libraries

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    MESSAGE("Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif ()
find_package(BZip2)
find_package(ZLIB)


find_package(PythonInterp 2.7)

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}} ${${_HEADER_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${ATLAS_ABI_VERSION}
            SOVERSION ${ATLAS_SOVERSION}
            )
    target_compile_options(${_LIB_NAME} PRIVATE ${WF_WARNING_FLAGS})

    install(TARGETS ${_LIB_NAME}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

endmacro()

if (PYTHONINTERP_FOUND)

    set(PY_SOURCES
            tools/generate/gen_cpp.py
            tools/generate/common.py
            tools/generate/AttributeInfo.py
            tools/generate/GenerateObjectFactory.py
            tools/generate/GenerateForward.py
            data/protocol/spec/xml/atlas.xml)

    set(ATLAS_XML_SOURCES
            data/protocol/spec/entity.def
            data/protocol/spec/root.def
            data/protocol/spec/operation.def
            data/protocol/spec/type.def
            data/protocol/spec/tools/def2xml.py
            )

    add_custom_target(GenerateAtlasXml
            COMMAND ${CMAKE_COMMAND} -E echo "Generating atlas.xml file from .def files."
            COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/src/Atlas-Python ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/data/protocol/spec/tools/def2xml.py ${CMAKE_SOURCE_DIR}/data/protocol/spec/root.def ${CMAKE_SOURCE_DIR}/data/protocol/spec/entity.def ${CMAKE_SOURCE_DIR}/data/protocol/spec/operation.def ${CMAKE_SOURCE_DIR}/data/protocol/spec/type.def ${CMAKE_SOURCE_DIR}/data/protocol/spec/xml/atlas.xml
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data/protocol/spec
            DEPENDS ${ATLAS_XML_SOURCES}
            VERBATIM
            )

    add_custom_target(GenerateAtlasObjects
            COMMAND ${CMAKE_COMMAND} -E echo "Generating Atlas Object sources."
            COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/generate ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/generate/gen_cpp.py ${CMAKE_SOURCE_DIR}/data/protocol/spec/xml/atlas.xml
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/Atlas/Objects
            DEPENDS ${PY_SOURCES}
            VERBATIM
            )
else (PYTHONINTERP_FOUND)
    message("Could not find any suitable Python environment. This just means that the automatic code generation features won't be available. Unless you plan on changing the protocol this should not be an issue.")
endif (PYTHONINTERP_FOUND)

add_subdirectory(src)


# pkg-config files
configure_file(tools/atlascpp.pc.in atlascpp${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/atlascpp${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# man files
install(FILES docs/man/man1/atlas_convert.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

# RPM spec files
configure_file(tools/atlas-cpp.spec.in atlas-cpp.spec @ONLY)
configure_file(tools/mingw32-atlas-cpp.spec.in mingw32-atlas-cpp.spec @ONLY)

# Add test
enable_testing()

# Add a "check" target, which builds and runs the tests.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -E Benchmark)

# Add a "benchmark" target, which builds and runs the benchmarks.
add_custom_target(benchmark COMMAND ${CMAKE_CTEST_COMMAND} -R Benchmark)

#Macro for adding a test. The test name will be extracted from the name of the first submitted file.
#Additional files can be submitted as varargs.
macro(wf_add_test TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    target_link_libraries(${TEST_NAME} AtlasObjects${SUFFIX} AtlasFilters${SUFFIX} AtlasCodecs${SUFFIX} AtlasMessage${SUFFIX} Atlas${SUFFIX})
    target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})
endmacro()

macro(wf_add_benchmark TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    target_link_libraries(${TEST_NAME} AtlasObjects${SUFFIX} AtlasFilters${SUFFIX} AtlasCodecs${SUFFIX} AtlasMessage${SUFFIX} Atlas${SUFFIX})
    target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    add_test(NAME ${TEST_NAME}Benchmark COMMAND ${TEST_NAME})

    add_dependencies(benchmark ${TEST_NAME})
endmacro()

wf_add_test(tests/Codecs/codecs.cpp)
wf_add_test(tests/Objects/custom_ops.cpp)
wf_add_test(tests/Objects/objects1.cpp)
wf_add_test(tests/Objects/objects2.cpp tests/Objects/DebugBridge.h)
wf_add_test(tests/Objects/encoder1.cpp)
wf_add_test(tests/Objects/decoder1.cpp)
wf_add_test(tests/Objects/objects_fwd.cpp)
wf_add_test(tests/Objects/attributes.cpp)
wf_add_test(tests/Objects/flags.cpp)
add_definitions(-DTEST_ATLAS_XML_PATH="${PROJECT_SOURCE_DIR}/data/protocol/spec/xml/atlas.xml")

wf_add_benchmark(tests/benchmark/Objects_asMessage.cpp)
wf_add_benchmark(tests/benchmark/Objects3_Move.cpp)
wf_add_benchmark(tests/benchmark/Call_Move.cpp)
wf_add_benchmark(tests/benchmark/Message_Move.cpp)
wf_add_benchmark(tests/benchmark/Static_Move.cpp)
wf_add_benchmark(tests/benchmark/Objects_iterator.cpp)
wf_add_benchmark(tests/benchmark/Codecs_Packed.cpp)
wf_add_benchmark(tests/benchmark/Message_Element.cpp)
wf_add_benchmark(tests/benchmark/Objects_setAttr.cpp)


# Doxygen support, exports a "dox" target.

find_package(Doxygen)

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
        COMMAND ${CMAKE_SOURCE_DIR}/tools/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} 8bd480b053190ffde2afe33af66f484953036f5a
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
set(CPACK_PACKAGE_VERSION_MAJOR "${ATLAS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ATLAS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${ATLAS_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/tools/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)
