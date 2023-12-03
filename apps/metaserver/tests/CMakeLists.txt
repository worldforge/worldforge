include_directories(../src/server ../src/api)

add_executable(MetaServer_unittest
        MetaServer_unittest.cpp
        ../src/server/MetaServer.cpp
        ../src/api/MetaServerPacket.cpp
        ../src/server/DataObject.cpp)
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
        ../src/api/MetaServerPacket.cpp)
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
        ../src/server/DataObject.cpp)
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
        ../src/server/MetaServerHandlerUDP.cpp
        ../src/api/MetaServerPacket.cpp)
target_link_libraries(MetaServerHandlerUDP_unittest PUBLIC
        cppunit::cppunit
        Boost::program_options
        Boost::filesystem
        spdlog::spdlog
)
add_test(NAME MetaServerHandlerUDP_unittest COMMAND MetaServerHandlerUDP_unittest)
add_dependencies(check MetaServerHandlerUDP_unittest)
