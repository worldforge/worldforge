configure_file(buildid.cpp.in buildid.cpp)

add_library(cyphesis-comm
        CommMetaClient.cpp
        CommMDNSPublisher.cpp
        IdleConnector.cpp
)

target_link_libraries(cyphesis-comm PUBLIC
        cyphesis-common
        metaserver-api
)

if (AVAHI_FOUND)
    #For some reason it seems we need to also link to avahi-common
    target_link_libraries(cyphesis-comm PUBLIC
            ${AVAHI_LIBRARIES}
            -lavahi-common)
    add_compile_definitions(HAVE_AVAHI=1)
endif (AVAHI_FOUND)

set(server_files
        Account.cpp
        Player.cpp
        Admin.cpp
        SystemAccount.cpp
        ServerAccount.cpp
        Persistence.cpp
        EntityFactory.cpp
        ServerRouting.cpp
        StorageManager.cpp
        Ruleset.cpp
        EntityRuleHandler.cpp
        ArchetypeRuleHandler.cpp
        ArchetypeFactory.cpp
        OpRuleHandler.cpp
        PropertyRuleHandler.cpp
        EntityBuilder.cpp
        Lobby.cpp
        Connection.cpp
        TrustedConnection.cpp
        Peer.cpp
        Juncture.cpp
        TeleportState.cpp
        PossessionAuthenticator.cpp
        TeleportProperty.cpp
        PendingPossession.cpp
        CommPeer.cpp
        CommPythonClient.cpp
        ExternalMindsConnection.cpp
        ExternalMindsManager.cpp
        ServerPropertyManager.cpp
        MindProperty.cpp
        TypeUpdateCoordinator.cpp
        ScriptReloader.cpp
        AccountProperty.cpp
        SquallAssetsGenerator.cpp
        AssetsHandler.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/buildid.cpp
)

add_library(cyphesis-server
        ${server_files})

target_link_libraries(cyphesis-server PUBLIC
        cyphesis-common
        cyphesis-pythonbase
        squallcore
)


add_executable(cyphesis
        server.cpp
)

#If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
if (PYTHON_IS_STATIC)
    set_property(TARGET cyphesis PROPERTY ENABLE_EXPORTS 1)
endif ()

if (PostgreSQL_FOUND)
    target_compile_definitions(cyphesis PRIVATE POSTGRES_FOUND)
endif (PostgreSQL_FOUND)

target_link_libraries(cyphesis
        cyphesis-server
        cyphesis-rulessimulation_python
        cyphesis-rulessimulation
        cyphesis-rulesbase
        cyphesis-atlas_python
        cyphesis-common_python
        cyphesis-physics_python
        cyphesis-modules
        cyphesis-common
        cyphesis-db
        cyphesis-physics
        cyphesis-comm
        external-remotery
)

install(TARGETS cyphesis DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})


#add_executable(cyslave
#        Account.cpp
#        Player.cpp
#        Admin.cpp
#        Persistence.cpp
#        EntityFactory.cpp
#        ServerRouting.cpp
#        WorldRouter.cpp
#        TaskFactory.cpp
#        CorePropertyManager.cpp
#        EntityBuilder.cpp
#        Lobby.cpp
#        Connection.cpp
#        TrustedConnection.cpp
#        SlaveClientConnection.cpp
#        Peer.cpp
#        Master.cpp
#        HttpHandling.cpp
#        CommPeer.cpp
#        CommHttpClient.cpp
#        CommMaster.cpp
#        TeleportState.cpp
#        slave.cpp)
#
#target_link_libraries(cyslave
#        scriptpython
#        navigation
#        DetourTileCache
#        Detour
#        Recast
#        rulesetmind
#        rulessimulation
#        entityfilter
#        rulesbase
#        modules
#        common
#        physics
#        metaserverapi
#        comm)
