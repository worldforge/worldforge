add_executable(cycmd
        cycmd.cpp
        Flusher.cpp
        OperationMonitor.cpp
        EntityExporterBase.cpp
        EntityExporter.cpp
        EntityImporterBase.cpp
        EntityImporter.cpp
        AdminClient.cpp
        IdContext.cpp
        AccountContext.cpp
        AvatarContext.cpp
        ConnectionContext.cpp
        JunctureContext.cpp
        Interactive.cpp
)

target_link_libraries(cycmd PUBLIC
        cyphesis-common
        readline::readline
)

install(TARGETS cycmd DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cyconfig cyconfig.cpp)
target_link_libraries(cyconfig PUBLIC
        cyphesis-common
)
install(TARGETS cyconfig DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cypasswd
        cypasswd.cpp
        DatabaseCreation.cpp
)
target_link_libraries(cypasswd PUBLIC
        cyphesis-db
        cyphesis-common
)
install(TARGETS cypasswd DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cydb
        cydb.cpp
        DatabaseCreation.cpp
)

target_link_libraries(cydb
        readline::readline
        cyphesis-db
        cyphesis-common
)
install(TARGETS cydb DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cypython cypython.cpp)
target_link_libraries(cypython
        readline::readline
        cyphesis-common
)
install(TARGETS cypython DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cyexport
        cyexport.cpp
        EntityExporterBase.cpp
        EntityExporter.cpp
        AgentCreationTask.cpp
        WaitForDeletionTask.cpp
)
target_link_libraries(cyexport PUBLIC
        cyphesis-common
)
install(TARGETS cyexport DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

add_executable(cyimport
        cyimport.cpp
        EntityImporterBase.cpp
        EntityImporter.cpp
        AgentCreationTask.cpp
        EntityTraversalTask.cpp
        WaitForDeletionTask.cpp
)
target_link_libraries(cyimport
        cyphesis-common)
install(TARGETS cyimport DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

