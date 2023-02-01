cmake_minimum_required(VERSION 3.23)

project(eris)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
include(GNUInstallDirs)
include(FindPkgConfig)

# Version setup

set(VERSION_MAJOR 1)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(ABI_CURRENT 0)
set(ABI_REVISION 0)
set(ABI_AGE 0)
math(EXPR SOVERSION ${ABI_CURRENT}-${ABI_AGE})
set(ABI_VERSION ${SOVERSION}.${ABI_AGE}.${ABI_REVISION})

option(BUILD_TESTING "Should tests always be built; otherwise they will be built when the 'check' target is executed." OFF)
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static." ON)

# Set compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WF_WARNING_FLAGS /W3)
    add_definitions(-D_WIN32_WINNT=0x0601) #target Windows 7
else ()
    set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)
endif ()

include_directories("${PROJECT_SOURCE_DIR}/src")

# Meta data

set(LIBNAME "${PROJECT_NAME}")
set(DESCRIPTION "A WorldForge client library.")

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${ABI_VERSION}
            SOVERSION ${SOVERSION}
            )
    target_compile_options(${_LIB_NAME} PUBLIC ${WF_WARNING_FLAGS})
    target_sources(${_LIB_NAME} PUBLIC FILE_SET headers TYPE HEADERS FILES
            ${${_HEADER_FILES_VAR}})
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
            FILE_SET headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            )

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

    #Link to the same libs as the main lib without linking to the lib itself.
    get_target_property(LINKED_LIBS ${LIBNAME} LINK_LIBRARIES)
    target_link_libraries(${TEST_NAME}
            ${LINKED_LIBS}
            )

    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    #We need to tell adjust the path so tests on windows can find the .dll files.
    SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/bin;$ENV{PATH}")

    add_dependencies(check ${TEST_NAME})
endmacro()

macro(wf_add_test_linked TEST_FILE)
    wf_add_test(${TEST_FILE} ${ARGN})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME})
endmacro()


if (EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
    message(STATUS "Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
    if (${CONAN_CMAKE_POSITION_INDEPENDENT_CODE})
        message(STATUS "Enabling position independent code")
        set(CMAKE_POSITION_INDEPENDENT_CODE ${CONAN_CMAKE_POSITION_INDEPENDENT_CODE})
    endif ()
endif ()

find_package(sigc++
        2.0
        REQUIRED)


find_package(Atlas
        0.7.0
        REQUIRED)


find_package(wfmath
        1.0.3
        REQUIRED)


find_package(Boost
        1.46.0
        REQUIRED
        COMPONENTS system headers)


#boost::asio on unix systems requires pthreads, but that's not always picked up, so we need to declare it.
if (UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif (UNIX)


# Populate for pkg-config
set(REQUIRES "sigc++-2.0 atlascpp-0.7 wfmath-1.0")

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

# pkg-config files
configure_file(tools/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Doxygen support, exports a "dox" target.
configure_file(docs/Doxyfile.in Doxyfile @ONLY)

find_package(Doxygen)

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
        COMMAND ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} 6951ccbc623ff06dc9598bef83c3d820f9b51aec
)
add_custom_target(changelog DEPENDS ChangeLog)


# CMake config files
include(CMakePackageConfigHelpers)

install(EXPORT "${PROJECT_NAME}Targets"
        FILE "${PROJECT_NAME}Targets.cmake"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        )
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
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)