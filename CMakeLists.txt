cmake_minimum_required(VERSION 3.2)
project(Atlas-C++)
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
set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)

include_directories("${PROJECT_SOURCE_DIR}")

# Meta data

set(DESCRIPTION "Networking protocol for the Worldforge system.")

# Define source files for libraries

set(CODECS_SOURCE_FILES
        Atlas/Codecs/Bach.cpp
        Atlas/Codecs/Packed.cpp
        Atlas/Codecs/XML.cpp)

set(CODECS_HEADER_FILES
        Atlas/Codecs/Bach.h
        Atlas/Codecs/Packed.h
        Atlas/Codecs/Utility.h
        Atlas/Codecs/XML.h)

set(OBJECTS_SOURCE_FILES
        Atlas/Objects/Anonymous.cpp
        Atlas/Objects/BaseObject.cpp
        Atlas/Objects/Decoder.cpp
        Atlas/Objects/Generic.cpp
        Atlas/Objects/loadDefaults.cpp
        Atlas/Objects/Factories_generated.cpp
        Atlas/Objects/Factories.cpp
        Atlas/Objects/Root.cpp
        Atlas/Objects/RootEntity.cpp
        Atlas/Objects/RootEntityChildren.cpp
        Atlas/Objects/RootOperation.cpp
        Atlas/Objects/RootOperationChildren1.cpp
        Atlas/Objects/RootOperationChildren2.cpp
        Atlas/Objects/RootOperationChildren3.cpp
        Atlas/Objects/RootOperationChildren4.cpp
        Atlas/Objects/RootOperationChildren5.cpp
        Atlas/Objects/RootOperationChildren6.cpp
        )

set(OBJECTS_HEADER_FILES
        Atlas/Objects/Anonymous.h
        Atlas/Objects/BaseObject.h
        Atlas/Objects/Decoder.h
        Atlas/Objects/Encoder.h
        Atlas/Objects/Entity.h
        Atlas/Objects/Generic.h
        Atlas/Objects/loadDefaults.h
        Atlas/Objects/Factories.h
        Atlas/Objects/ObjectsFwd.h
        Atlas/Objects/Operation.h
        Atlas/Objects/Root.h
        Atlas/Objects/RootEntity.h
        Atlas/Objects/RootOperation.h
        Atlas/Objects/SmartPtr.h)

set(FILTERS_SOURCE_FILES
        Atlas/Filters/Bzip2.cpp
        Atlas/Filters/Gzip.cpp)

set(FILTERS_HEADER_FILES
        Atlas/Filters/Bzip2.h
        Atlas/Filters/Gzip.h)

set(FUNKY_SOURCE_FILES
        Atlas/Funky/FEncoder.cpp)

set(FUNKY_HEADER_FILES
        Atlas/Funky/FEncoder.h)

set(MESSAGE_SOURCE_FILES
        Atlas/Message/DecoderBase.cpp
        Atlas/Message/Element.cpp
        Atlas/Message/MEncoder.cpp
        Atlas/Message/QueuedDecoder.cpp)

set(MESSAGE_HEADER_FILES
        Atlas/Message/DecoderBase.h
        Atlas/Message/Element.h
        Atlas/Message/MEncoder.h
        Atlas/Message/QueuedDecoder.h)

set(NET_SOURCE_FILES
        Atlas/Net/Loopback.cpp
        Atlas/Net/Stream.cpp)

set(NET_HEADER_FILES
        Atlas/Net/Loopback.h
        Atlas/Net/Stream.h)

set(SOURCE_FILES
        Atlas/Codec.h
        Atlas/EncoderBase.h
        Atlas/Filter.cpp
        Atlas/float.cpp
        Atlas/Formatter.cpp
        Atlas/MultiLineListFormatter.cpp
        Atlas/PresentationBridge.cpp)

set(HEADER_FILES
        Atlas/Bridge.h
        Atlas/Codec.h
        Atlas/Debug.h
        Atlas/EncoderBase.h
        Atlas/Exception.h
        Atlas/Filter.h
        Atlas/float.h
        Atlas/Formatter.h
        Atlas/MultiLineListFormatter.h
        Atlas/Negotiate.h
        Atlas/PresentationBridge.h
        Atlas/Version.h)

find_package(PythonInterp 2.7)

if (PYTHON_EXECUTABLE)

    set(PY_SOURCES
            Atlas/Objects/gen_cpp.py
            Atlas/Objects/common.py
            Atlas/Objects/AttributeInfo.py
            Atlas/Objects/GenerateObjectFactory.py
            Atlas/Objects/GenerateForward.py
            protocol/spec/xml/atlas.xml)

    set(ATLAS_XML_SOURCES
            protocol/spec/entity.def
            protocol/spec/root.def
            protocol/spec/operation.def
            protocol/spec/type.def
            protocol/spec/tools/def2xml.py
            )

    add_custom_target(GenerateAtlasXml
            COMMAND ${CMAKE_COMMAND} -E echo "Generating atlas.xml file from .def files."
            COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/Atlas-Python ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/protocol/spec/tools/def2xml.py ${CMAKE_SOURCE_DIR}/protocol/spec/root.def ${CMAKE_SOURCE_DIR}/protocol/spec/entity.def ${CMAKE_SOURCE_DIR}/protocol/spec/operation.def ${CMAKE_SOURCE_DIR}/protocol/spec/type.def ${CMAKE_SOURCE_DIR}/protocol/spec/xml/atlas.xml
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/protocol/spec
            DEPENDS ${ATLAS_XML_SOURCES}
            VERBATIM
            )

    add_custom_target(GenerateAtlasObjects
            COMMAND ${CMAKE_COMMAND} -E echo "Generating Atlas Object sources."
            COMMAND ${PYTHON_EXECUTABLE} gen_cpp.py ${CMAKE_SOURCE_DIR}/protocol/spec/xml/atlas.xml
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Atlas/Objects
            DEPENDS ${PY_SOURCES}
            VERBATIM
    )

endif (PYTHON_EXECUTABLE)

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} SHARED ${${_SOURCE_FILES_VAR}} ${${_HEADER_FILES_VAR}})
    add_library(${_LIB_NAME}_STATIC STATIC ${${_SOURCE_FILES_VAR}} ${${_HEADER_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${ATLAS_ABI_VERSION}
            SOVERSION ${ATLAS_SOVERSION}
            )

    install(TARGETS ${_LIB_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    foreach (file ${${_HEADER_FILES_VAR}})
        get_filename_component(dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${PROJECT_NAME}${SUFFIX}/${dir})
    endforeach ()

    set(PKG_CONFIG_LIBS "-l${_LIB_NAME} ${PKG_CONFIG_LIBS}")

endmacro()


wf_add_library(Atlas${SUFFIX} SOURCE_FILES HEADER_FILES)
target_compile_options(Atlas${SUFFIX} PRIVATE ${WF_WARNING_FLAGS})

wf_add_library(AtlasCodecs${SUFFIX} CODECS_SOURCE_FILES CODECS_HEADER_FILES)
target_compile_options(AtlasCodecs${SUFFIX} PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(AtlasCodecs${SUFFIX} Atlas${SUFFIX})

wf_add_library(AtlasNet${SUFFIX} NET_SOURCE_FILES NET_HEADER_FILES)
target_compile_options(AtlasNet${SUFFIX} PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(AtlasNet${SUFFIX} Atlas${SUFFIX} AtlasCodecs${SUFFIX})

wf_add_library(AtlasMessage${SUFFIX} MESSAGE_SOURCE_FILES MESSAGE_HEADER_FILES)
target_compile_options(AtlasMessage${SUFFIX} PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(AtlasMessage${SUFFIX} Atlas${SUFFIX})

wf_add_library(AtlasObjects${SUFFIX} OBJECTS_SOURCE_FILES OBJECTS_HEADER_FILES)
target_compile_options(AtlasObjects${SUFFIX} PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(AtlasObjects${SUFFIX} AtlasMessage${SUFFIX} AtlasCodecs${SUFFIX})
if (PYTHON_EXECUTABLE)
    add_dependencies(AtlasObjects${SUFFIX} GenerateAtlasXml GenerateAtlasObjects)
    add_dependencies(GenerateAtlasObjects GenerateAtlasXml)
endif (PYTHON_EXECUTABLE)

wf_add_library(AtlasFilters${SUFFIX} FILTERS_SOURCE_FILES FILTERS_HEADER_FILES)
target_compile_options(AtlasFilters${SUFFIX} PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(AtlasFilters${SUFFIX} Atlas${SUFFIX})

# The "atlas_convert" tool
add_executable(atlas_convert tools/atlas_convert.cpp)
target_compile_options(atlas_convert PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(atlas_convert AtlasCodecs${SUFFIX} AtlasMessage${SUFFIX} Atlas${SUFFIX})
install(TARGETS atlas_convert
        DESTINATION ${CMAKE_INSTALL_BINDIR})

# pkg-config files
configure_file(atlascpp.pc.in atlascpp${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/atlascpp${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# man files
install(FILES man/man1/atlas_convert.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

# RPM spec files
configure_file(atlas-cpp.spec.in atlas-cpp.spec @ONLY)
configure_file(mingw32-atlas-cpp.spec.in mingw32-atlas-cpp.spec @ONLY)


# Check for libraries

find_package(BZip2)
if (BZIP2_FOUND)
    include_directories(${BZIP2_INCLUDE_DIRS})
    target_link_libraries(AtlasFilters${SUFFIX} ${BZIP2_LIBRARIES})
    add_definitions(-DHAVE_BZLIB_H -DHAVE_LIBBZ2)
endif (BZIP2_FOUND)

find_package(ZLIB)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(AtlasFilters${SUFFIX} ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_ZLIB_H -DHAVE_LIBZ)
endif (ZLIB_FOUND)

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
    target_link_libraries(${TEST_NAME} AtlasObjects${SUFFIX}_STATIC AtlasFilters${SUFFIX}_STATIC AtlasCodecs${SUFFIX}_STATIC AtlasMessage${SUFFIX}_STATIC Atlas${SUFFIX}_STATIC)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(check ${TEST_NAME})
endmacro()

macro(wf_add_benchmark TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    target_link_libraries(${TEST_NAME} AtlasObjects${SUFFIX}_STATIC AtlasFilters${SUFFIX}_STATIC AtlasCodecs${SUFFIX}_STATIC AtlasMessage${SUFFIX}_STATIC Atlas${SUFFIX}_STATIC)
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
add_definitions(-DTEST_ATLAS_XML_PATH="${PROJECT_SOURCE_DIR}/protocol/spec/xml/atlas.xml")

wf_add_benchmark(benchmark/Objects_asMessage.cpp)
wf_add_benchmark(benchmark/Objects3_Move.cpp)
wf_add_benchmark(benchmark/Call_Move.cpp)
wf_add_benchmark(benchmark/Message_Move.cpp)
wf_add_benchmark(benchmark/Static_Move.cpp)
wf_add_benchmark(benchmark/Objects_iterator.cpp)
wf_add_benchmark(benchmark/Codecs_Packed.cpp)
wf_add_benchmark(benchmark/Message_Element.cpp)
wf_add_benchmark(benchmark/Objects_setAttr.cpp)


# Doxygen support, exports a "docs" target.

find_package(Doxygen)

if (DOXYGEN_FOUND)

    set(DOXYGEN_INPUT Doxyfile)
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
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${ATLAS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ATLAS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${ATLAS_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)
