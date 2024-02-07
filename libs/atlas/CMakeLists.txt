project(Atlas)

# Meta data

set(DESCRIPTION "Networking protocol for the Worldforge system.")

# Check for libraries

find_package(BZip2)
find_package(ZLIB)


find_package(Python3 COMPONENTS Interpreter)
cmake_dependent_option(ATLAS_GENERATE_OBJECTS "Generate Atlas Object classes from Atlas defs" TRUE "Python3_EXECUTABLE AND NOT WIN32" FALSE)
option(ATLAS_DISABLE_BENCHMARKS "Disable benchmarks." FALSE)


# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}})

    target_compile_options(${_LIB_NAME} PUBLIC ${WF_WARNING_FLAGS})
    target_compile_features(${_LIB_NAME} PUBLIC cxx_std_20)
    target_include_directories(${_LIB_NAME}
            PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

endmacro()

if (ATLAS_GENERATE_OBJECTS)

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
            COMMAND PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas-Python ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/tools/def2xml.py ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/root.def ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/entity.def ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/operation.def ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/type.def ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/xml/atlas.xml
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec
            DEPENDS ${ATLAS_XML_SOURCES}
            VERBATIM
    )

    add_custom_target(GenerateAtlasObjects
            COMMAND ${CMAKE_COMMAND} -E echo "Generating Atlas Object sources."
            COMMAND PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas-Python ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate/gen_cpp.py ${CMAKE_CURRENT_SOURCE_DIR}/data/protocol/spec/xml/atlas.xml
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/Atlas/Objects
            DEPENDS ${PY_SOURCES}
            VERBATIM
    )
    message(STATUS "Added targets for generating Atlas code.")
else (ATLAS_GENERATE_OBJECTS)
    if (NOT Python3_EXECUTABLE)
        message(STATUS "Could not find any suitable Python environment. This just means that the automatic code generation features won't be available. Unless you plan on changing the protocol this should not be an issue.")
    else ()
        message(STATUS "Atlas code generation disabled.")
    endif ()
endif (ATLAS_GENERATE_OBJECTS)

add_subdirectory(src)

if (NOT TARGET benchmark)
    add_custom_target(benchmark)
endif ()
if (NOT TARGET check)
    add_custom_target(check)
endif ()

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
    target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    #We need to tell adjust the path so tests on windows can find the .dll files.
    SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin;$ENV{PATH}")

    target_link_libraries(${TEST_NAME}
            AtlasObjects
            AtlasFilters
            AtlasCodecs
            AtlasMessage
            Atlas)

    add_dependencies(check ${TEST_NAME})
endmacro()

macro(wf_add_benchmark TEST_FILE)

    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} ${ARGN})
    target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")

    target_link_libraries(${TEST_NAME}
            AtlasObjects
            AtlasFilters
            AtlasCodecs
            AtlasMessage
            Atlas)

    #Always build, but activate only if actively running benchmarks.
    if (NOT ATLAS_DISABLE_BENCHMARKS)
        add_test(NAME ${TEST_NAME}Benchmark COMMAND ${TEST_NAME})
        #We need to tell adjust the path so tests on windows can find the .dll files.
        SET_TESTS_PROPERTIES(${TEST_NAME}Benchmark PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin;$ENV{PATH}")
        add_dependencies(benchmark ${TEST_NAME})
    endif ()
endmacro()

wf_add_test(tests/Message/ElementTest.cpp)
wf_add_test(tests/Codecs/codecs.cpp)
wf_add_test(tests/Objects/custom_ops.cpp)
wf_add_test(tests/Objects/objects1.cpp tests/Objects/loadDefaults.cpp)
wf_add_test(tests/Objects/objects2.cpp tests/Objects/DebugBridge.h tests/Objects/loadDefaults.cpp)
wf_add_test(tests/Objects/encoder1.cpp tests/Objects/loadDefaults.cpp)
wf_add_test(tests/Objects/decoder1.cpp tests/Objects/loadDefaults.cpp)
wf_add_test(tests/Objects/objects_fwd.cpp)
wf_add_test(tests/Objects/attributes.cpp)
wf_add_test(tests/Objects/flags.cpp)
add_compile_definitions("TEST_ATLAS_XML_PATH=\"${PROJECT_SOURCE_DIR}/data/protocol/spec/xml/atlas.xml\"")

wf_add_benchmark(tests/benchmark/Objects_asMessage.cpp)
wf_add_benchmark(tests/benchmark/Objects3_Move.cpp tests/Objects/loadDefaults.cpp)
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

    #    add_custom_target(dox DEPENDS ${DOXYGEN_OUTPUT})


endif (DOXYGEN_FOUND)

add_custom_command(
        OUTPUT ChangeLog
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/support/generate-ChangeLog.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} 8bd480b053190ffde2afe33af66f484953036f5a
)

#add_custom_target(changelog DEPENDS ChangeLog)
