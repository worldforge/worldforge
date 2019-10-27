cmake_minimum_required(VERSION 3.2)

project(eris)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
include(GNUInstallDirs)
include(FindPkgConfig)
include(FindBoost)

# Version setup

set(ERIS_VERSION_MAJOR 1)
set(ERIS_VERSION_MINOR 4)
set(ERIS_VERSION_PATCH 0)

set(VERSION ${ERIS_VERSION_MAJOR}.${ERIS_VERSION_MINOR}.${ERIS_VERSION_PATCH})
set(SUFFIX -${ERIS_VERSION_MAJOR}.${ERIS_VERSION_MINOR})

set(ERIS_ABI_CURRENT 0)
set(ERIS_ABI_REVISION 0)
set(ERIS_ABI_AGE 0)
math(EXPR ERIS_SOVERSION ${ERIS_ABI_CURRENT}-${ERIS_ABI_AGE})
set(ERIS_ABI_VERSION ${ERIS_SOVERSION}.${ERIS_ABI_AGE}.${ERIS_ABI_REVISION})

option(BUILD_TESTING "Should tests always be built; otherwise they will be built when the 'check' target is executed." OFF)
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static." ON)

# Set compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(WF_WARNING_FLAGS /W3)
else ()
    set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)
endif ()

include_directories("${PROJECT_SOURCE_DIR}/src")

# Meta data

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
set(LIBNAME "${PROJECT_NAME_LOWER}${SUFFIX}")
set(DESCRIPTION "A WorldForge client library.")

# We've been getting issues with memory corruption in boost when building with GCC -O2 flags, and it
# seems to be related to the asio small memory block recycling code. We'll thus disable it to avoid the
# crashes
add_definitions(-DBOOST_ASIO_DISABLE_SMALL_BLOCK_RECYCLING)

# This macro defines a library
macro(wf_add_library _LIB_NAME _SOURCE_FILES_VAR _HEADER_FILES_VAR)

    add_library(${_LIB_NAME} ${${_SOURCE_FILES_VAR}} ${${_HEADER_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${ERIS_ABI_VERSION}
            SOVERSION ${ERIS_SOVERSION}
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
    target_link_libraries(${TEST_NAME} ${WF_LIBRARIES})
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)

    add_dependencies(check ${TEST_NAME})
endmacro()

macro(wf_add_test_linked TEST_FILE)
    wf_add_test(${TEST_FILE} ${ARGN})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME_LOWER}${SUFFIX})
endmacro()

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

find_package(Atlas
        0.7.0
        REQUIRED)
list(APPEND WF_LIBRARIES ${Atlas_LIBRARIES})
list(APPEND WF_INCLUDE_DIRS ${Atlas_INCLUDE_DIR})
list(APPEND WF_LIBRARY_DIRS ${Atlas_LIBRARY_DIR})

find_package(wfmath
        1.0.3
        REQUIRED)
list(APPEND WF_LIBRARIES ${wfmath_LIBRARIES})
list(APPEND WF_INCLUDE_DIRS ${wfmath_INCLUDE_DIR})
list(APPEND WF_LIBRARY_DIRS ${wfmath_LIBRARY_DIR})


find_package(Boost
        1.46.0
        REQUIRED
        COMPONENTS system)

list(APPEND WF_LIBRARIES ${Boost_LIBRARIES})
list(APPEND WF_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
list(APPEND WF_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})

#boost::asio on unix systems requires pthreads, but that's not always picked up, so we need to declare it.
if (UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif (UNIX)


link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
# Populate for pkg-config
set(REQUIRES "sigc++-2.0 atlascpp-0.7 wfmath-1.0")

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

# pkg-config files
configure_file(tools/${PROJECT_NAME_LOWER}.pc.in ${PROJECT_NAME_LOWER}${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RPM spec files
#TODO: fix these
configure_file(tools/${PROJECT_NAME_LOWER}.spec.in ${PROJECT_NAME_LOWER}.spec @ONLY)
#TODO: fix these
configure_file(tools/mingw32-${PROJECT_NAME_LOWER}.spec.in mingw32-${PROJECT_NAME_LOWER}.spec @ONLY)

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
set(CPACK_PACKAGE_VERSION_MAJOR "${ERIS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ERIS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${ERIS_VERSION_PATCH}")
#set(CPACK_INSTALL_SCRIPT "sh ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CPACK_PACKAGE_INSTALL_DIRECTORY} 8bd480b053190ffde2afe33af66f484953036f5a")

set(CPACK_SOURCE_GENERATOR TBZ2 ZIP)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION}" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
        # no hidden files
        "/\\\\..+$"
        "~$"
        )

include(CPack)