wf_find_boost(program_options)

add_executable(metaserver
        main.cpp
        MetaServer.cpp
        MetaServerHandlerUDP.cpp
        DataObject.cpp
)
target_link_libraries(metaserver
        metaserver-api
        Boost::program_options
        spdlog::spdlog
)
install(TARGETS metaserver DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})


add_executable(ms-testserver
        TestServer.cpp
)
target_link_libraries(ms-testserver
        metaserver-api
        Boost::program_options
        spdlog::spdlog
)


add_executable(ms-testclient
        TestClient.cpp
)
target_link_libraries(ms-testclient
        metaserver-api
        Boost::program_options
        spdlog::spdlog
)


add_executable(ms-pdnspipe
        PDNSPipe.cpp
)
target_link_libraries(ms-pdnspipe
        metaserver-api
        Boost::program_options
        spdlog::spdlog
)
