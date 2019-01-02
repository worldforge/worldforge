cmake_minimum_required(VERSION 3.2)

project(Eris)
set(CMAKE_CXX_STANDARD 14)
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

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories("${PROJECT_SOURCE_DIR}")

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

    add_library(${_LIB_NAME} SHARED ${${_SOURCE_FILES_VAR}} ${${_HEADER_FILES_VAR}})
    add_library(${_LIB_NAME}_STATIC STATIC ${${_SOURCE_FILES_VAR}})
    set_target_properties(${_LIB_NAME} PROPERTIES
            VERSION ${ERIS_ABI_VERSION}
            SOVERSION ${ERIS_SOVERSION}
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


pkg_check_modules(WF REQUIRED sigc++-2.0>=2.0.0 atlascpp-0.7>=0.7.0 wfmath-1.0>=1.0.0)

link_directories(${WF_LIBRARY_DIRS})
include_directories(${WF_INCLUDE_DIRS})
# Populate for pkg-config
set(REQUIRES "sigc++-2.0 atlascpp-0.7 wfmath-1.0")



find_package(Boost
        1.46.0
        REQUIRED
        COMPONENTS system)

link_directories(${Boost_LIBRARY_DIRS})
link_libraries(${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})

#boost::asio on unix systems requires pthreads, but that's not always picked up, so we need to declare it.
if (UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif (UNIX)


# Define source files for libraries

set(SOURCE_FILES
        Eris/Account.cpp
        Eris/Avatar.cpp
        Eris/BaseConnection.cpp
        Eris/Calendar.cpp
        Eris/CharacterType.cpp
        Eris/Connection.cpp
        Eris/CustomEntities.cpp
        Eris/Entity.cpp
        Eris/EntityRef.cpp
        Eris/EntityRouter.cpp
        Eris/EventService.cpp
        Eris/Factory.cpp
        Eris/IGRouter.cpp
        Eris/Lobby.cpp
        Eris/Log.cpp
        Eris/MetaQuery.cpp
        Eris/Metaserver.cpp
        Eris/Operations.cpp
        Eris/Person.cpp
        Eris/Redispatch.cpp
        Eris/Response.cpp
        Eris/Room.cpp
        Eris/Router.cpp
        Eris/ServerInfo.cpp
        Eris/Session.cpp
        Eris/SpawnPoint.cpp
        Eris/StreamSocket.cpp
        Eris/Task.cpp
        Eris/TransferInfo.cpp
        Eris/TypeBoundRedispatch.cpp
        Eris/TypeInfo.cpp
        Eris/TypeService.cpp
        Eris/View.cpp
        Eris/ViewEntity.cpp
        Eris/ActiveMarker.cpp)

set(HEADER_FILES
        Eris/Account.h
        Eris/Avatar.h
        Eris/BaseConnection.h
        Eris/Calendar.h
        Eris/CharacterType.h
        Eris/Connection.h
        Eris/CustomEntities.h
        Eris/Entity.h
        Eris/EntityRef.h
        Eris/EntityRouter.h
        Eris/EventService.h
        Eris/Exceptions.h
        Eris/Factory.h
        Eris/IGRouter.h
        Eris/iround.h
        Eris/Lobby.h
        Eris/Log.h
        Eris/LogStream.h
        Eris/MetaQuery.h
        Eris/Metaserver.h
        Eris/Operations.h
        Eris/Person.h
        Eris/Redispatch.h
        Eris/Response.h
        Eris/Room.h
        Eris/Router.h
        Eris/ServerInfo.h
        Eris/Session.h
        Eris/SpawnPoint.h
        Eris/StreamSocket.h
        Eris/StreamSocket_impl.h
        Eris/Task.h
        Eris/TransferInfo.h
        Eris/TypeBoundRedispatch.h
        Eris/TypeInfo.h
        Eris/Types.h
        Eris/TypeService.h
        Eris/View.h
        Eris/ViewEntity.h
        Eris/WaitFreeQueue.h
        Eris/ActiveMarker.h)

wf_add_library(${LIBNAME} SOURCE_FILES HEADER_FILES)

# pkg-config files
configure_file(${PROJECT_NAME_LOWER}.pc.in ${PROJECT_NAME_LOWER}${SUFFIX}.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}${SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RPM spec files
#TODO: fix these
configure_file(${PROJECT_NAME_LOWER}.spec.in ${PROJECT_NAME_LOWER}.spec @ONLY)
#TODO: fix these
configure_file(mingw32-${PROJECT_NAME_LOWER}.spec.in mingw32-${PROJECT_NAME_LOWER}.spec @ONLY)

enable_testing()

wf_add_test(test/Account_integrationtest.cpp)
wf_add_test(test/Account_unittest.cpp)
wf_add_test(test/Avatar_unittest.cpp)
wf_add_test(test/BaseConnection_unittest.cpp)
wf_add_test(test/Calendar_unittest.cpp)
wf_add_test(test/Connection_unittest.cpp)
wf_add_test(test/DeleteLater_unittest.cpp)
wf_add_test(test/Entity_unittest.cpp)
wf_add_test(test/EntityRef_unittest.cpp)
wf_add_test(test/EntityRouter_unittest.cpp)
wf_add_test(test/EventService_unittest.cpp)
wf_add_test(test/Exceptions_unittest.cpp)
wf_add_test(test/Factory_unittest.cpp)
wf_add_test(test/IGRouter_unittest.cpp)
wf_add_test(test/Lobby_unittest.cpp)
wf_add_test(test/Log_unittest.cpp)
wf_add_test(test/LogStream_unittest.cpp)
wf_add_test(test/MetaQuery_unittest.cpp)
wf_add_test(test/Metaserver_integrationtest.cpp)
wf_add_test(test/Metaserver_unittest.cpp)
wf_add_test(test/Operations_unittest.cpp)
wf_add_test(test/Person_unittest.cpp)
wf_add_test(test/Poll_unittest.cpp)
wf_add_test(test/PollWinsock_unittest.cpp)
wf_add_test(test/Redispatch_unittest.cpp)
wf_add_test(test/Response_unittest.cpp)
wf_add_test(test/Room_unittest.cpp)
wf_add_test(test/Router_unittest.cpp)
wf_add_test(test/ServerInfo_unittest.cpp)
wf_add_test(test/Task_unittest.cpp)
wf_add_test(test/TransferInfo_unittest.cpp)
wf_add_test(test/TypeBoundRedispatch_unittest.cpp)
wf_add_test(test/TypeInfo_unittest.cpp)
wf_add_test(test/Types_unittest.cpp)
wf_add_test(test/TypeService_unittest.cpp)
wf_add_test(test/View_unittest.cpp)
wf_add_test(test/ActiveMarker_UnitTest.cpp)

#wf_add_test(testEris test/tests.cpp
#        test/stubServer.h test/stubServer.cpp
#        test/clientConnection.cpp test/clientConnection.h
#        test/agent.cpp test/agent.h
#        test/commander.cpp test/commander.h
#        test/controller.cpp test/controller.h
#        test/testUtils.h test/testUtils.cpp
#        test/objectSummary.h
#        test/netTests.cpp test/netTests.h
#        test/setupHelpers.cpp test/setupHelpers.h
#        test/signalHelpers.h
#        test/testOutOfGame.cpp test/testOutOfGame.h
#        test/viewTest.cpp test/viewTest.h
#        test/avatarTest.cpp test/avatarTest.h
#        test/calendarTest.cpp test/calendarTest.h)

add_executable(metaQuery test/metaQuery.cpp)
target_link_libraries(metaQuery ${LIBNAME})
add_executable(eris_connect test/connect.cpp)
target_link_libraries(eris_connect ${LIBNAME})

# Doxygen support, exports a "docs" target.

configure_file(eris.dox.in eris.dox @ONLY)

find_package(Doxygen)

if (DOXYGEN_FOUND)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/eris.dox)
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
        COMMAND ${CMAKE_SOURCE_DIR}/support/generate-ChangeLog.sh ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR} 6951ccbc623ff06dc9598bef83c3d820f9b51aec
)
add_custom_target(changelog DEPENDS ChangeLog)


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