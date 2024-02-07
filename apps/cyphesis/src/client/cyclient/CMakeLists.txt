add_library(cyphesis-cyclient
        CharacterClient.cpp
        CreatorClient.cpp
        BaseClientLegacy.cpp
        ObserverClient.cpp
        ClientConnection.cpp
        Python_ClientAPI.cpp
        CyPy_CreatorClient.cpp
        CyPy_ObserverClient.cpp)

target_link_libraries(cyphesis-cyclient
        cyphesis-client
        cyphesis-navigation
        external-DetourTileCache
        external-Detour
        external-Recast
        cyphesis-rulesai_python
        cyphesis-rulesai
        cyphesis-atlas_python
        cyphesis-common_python
        cyphesis-physics_python
        cyphesis-rulesbase
        cyphesis-modules
        cyphesis-common
        cyphesis-physics
)

add_executable(cyclient
        client.cpp
)

target_link_libraries(cyclient
        cyphesis-cyclient
)

install(TARGETS cyclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
