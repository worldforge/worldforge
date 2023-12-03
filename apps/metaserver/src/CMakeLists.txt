add_compile_definitions(SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")

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



add_executable(MetaServer_unittest
        MetaServer_unittest.cpp
        MetaServer.cpp
        MetaServerPacket.cpp
        DataObject.cpp)
target_link_libraries(MetaServer_unittest PUBLIC
        cppunit::cppunit
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)
add_test(NAME MetaServer_unittest COMMAND MetaServer_unittest)
add_dependencies(check MetaServer_unittest)


add_executable(MetaServerPackage_unittest
        MetaServerPacket_unittest.cpp
        MetaServerPacket.cpp)
target_link_libraries(MetaServerPackage_unittest PUBLIC
        cppunit::cppunit
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)
add_test(NAME MetaServerPackage_unittest COMMAND MetaServerPackage_unittest)
add_dependencies(check MetaServerPackage_unittest)


add_executable(DataObject_unittest
        DataObject_unittest.cpp
        DataObject.cpp)
target_link_libraries(DataObject_unittest PUBLIC
        cppunit::cppunit
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)
add_test(NAME DataObject_unittest COMMAND DataObject_unittest)
add_dependencies(check DataObject_unittest)


add_executable(MetaServerHandlerUDP_unittest
        MetaServerHandlerUDP_unittest.cpp
        MetaServerHandlerUDP.cpp
        MetaServerPacket.cpp)
target_link_libraries(MetaServerHandlerUDP_unittest PUBLIC
        cppunit::cppunit
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)
add_test(NAME MetaServerHandlerUDP_unittest COMMAND MetaServerHandlerUDP_unittest)
add_dependencies(check MetaServerHandlerUDP_unittest)
