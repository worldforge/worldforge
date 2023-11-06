project(wfmath)

include_directories("${PROJECT_SOURCE_DIR}/src")

# Meta data

set(DESCRIPTION "A math library for the Worldforge system.")

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}})
    target_compile_options(${_LIB_NAME} PUBLIC ${WF_WARNING_FLAGS})
    target_include_directories(${_LIB_NAME}
            PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

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
    target_link_libraries(${TEST_NAME} "${PROJECT_NAME}")
    target_compile_options(${TEST_NAME} PUBLIC "-w")
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    #We need to tell adjust the path so tests on windows can find the .dll files.
    SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin;$ENV{PATH}")

    add_dependencies(check ${TEST_NAME})
endmacro()

add_subdirectory(src)
add_subdirectory(tests)


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
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} 8bd480b053190ffde2afe33af66f484953036f5a
)
#add_custom_target(changelog DEPENDS ChangeLog)
