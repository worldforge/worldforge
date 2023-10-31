
add_executable(cyclient
        CharacterClient.cpp
        CreatorClient.cpp
        BaseClient.cpp
        ObserverClient.cpp
        ClientConnection.cpp
        Python_ClientAPI.cpp
        client.cpp
        CyPy_CreatorClient.cpp
        CyPy_ObserverClient.cpp)

target_link_libraries(cyclient
        cyphesis-client
        cyphesis-navigation
        cyphesis-DetourTileCache
        cyphesis-Detour
        cyphesis-Recast
        cyphesis-rulesai_python
        cyphesis-rulesai
        cyphesis-entityfilter_python
        cyphesis-entityfilter
        cyphesis-atlas_python
        cyphesis-common_python
        cyphesis-physics_python
        cyphesis-rulesbase_python
        cyphesis-rulesbase
        cyphesis-modules
        cyphesis-common
        cyphesis-physics
        )

install(TARGETS cyclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
