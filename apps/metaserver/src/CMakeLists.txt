add_executable(ms-testserver
        TestServer.cpp
        MetaServerPacket.cpp
)
target_link_libraries(ms-testserver
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)


add_executable(ms-testclient
        TestClient.cpp
        MetaServerPacket.cpp
)
target_link_libraries(ms-testclient
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)


add_executable(ms-pdnspipe
        PDNSPipe.cpp
        MetaServerPacket.cpp
)
target_link_libraries(ms-pdnspipe
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)

add_executable(metaserver
        main.cpp
        MetaServer.cpp
        MetaServerHandler.cpp
        MetaServerHandlerUDP.cpp
        MetaServerPacket.cpp
        DataObject.cpp
)
target_link_libraries(metaserver
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)
install(TARGETS metaserver DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

