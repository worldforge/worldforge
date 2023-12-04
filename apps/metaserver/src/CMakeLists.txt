add_subdirectory(api)
if (BUILD_METASERVER_SERVER)
    message(STATUS "Will build the MetaServer server too.")
    add_subdirectory(server)
endif ()




