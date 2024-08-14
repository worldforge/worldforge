if (AVAHI_FOUND)
    link_libraries(${AVAHI_LIBRARIES} -lavahi-common)
endif (AVAHI_FOUND)
if (PostgreSQL_FOUND)
    link_libraries(PostgreSQL::PostgreSQL)
endif (PostgreSQL_FOUND)

#Just link all libraries here.
link_libraries(
        cyphesis-external-pycxx
        Bullet::Bullet
        mercator
        wfmath
        Atlas
        AtlasCodecs
        AtlasNet
        AtlasMessage
        AtlasObjects
        AtlasFilters
        varconf
        libsigcpp::sigc++
        external-remotery
        readline::readline
        external-dirmonitor
        external-DetourTileCache
        external-DetourCrowd
        external-Detour
        external-Recast
        external-libxdg-basedir
        libgcrypt::libgcrypt
        spdlog::spdlog
)

link_directories(
        ${BULLET_LIBRARY_DIR}
)

include_directories(
        ${BULLET_INCLUDE_DIR}
)

#On platforms which supports it, link the "libSegFault" library which provides nice stack traces
find_library(SEGFAULT_LIBRARY SegFault)
if (SEGFAULT_LIBRARY)
    message(STATUS "Adding the SegFault library for nice stack traces for tests.")
    link_libraries(${SEGFAULT_LIBRARY})
endif (SEGFAULT_LIBRARY)


set(PYTHON_TESTS_LIBS
        cyphesis-rulesai_python
        cyphesis-rulessimulation_python
        cyphesis-physics_python
        cyphesis-atlas_python
        cyphesis-common_python
        cyphesis-pythonbase
)


#Macro for adding a test
macro(wf_add_test TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ../src/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    target_compile_definitions(${TEST_NAME} PUBLIC TESTDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\")

    #We load all symbols and allow for multiple instances. This allows us to override specific symbols in our tests.
    target_link_options(${TEST_NAME} PUBLIC "LINKER:-z,muldefs")
    target_link_libraries(${TEST_NAME} PUBLIC
            cyphesis-rulesai_python
            cyphesis-rulessimulation_python
            cyphesis-physics_python
            cyphesis-atlas_python
            cyphesis-common_python
            cyphesis-pythonbase

            cyphesis-server
            cyphesis-rulessimulation_python
            cyphesis-rulessimulation
            cyphesis-rulesbase
            cyphesis-rulesai_python
            cyphesis-rulesai
            cyphesis-atlas_python
            cyphesis-common_python
            cyphesis-physics_python
            cyphesis-modules
            cyphesis-common
            cyphesis-db
            cyphesis-physics
            cyphesis-comm

            cyphesis-cyaiclient
            cyphesis-cyclient
            cyphesis-client
            cyphesis-navigation
            cyphesis-physics
            #Need to include again to get symbols that were references from libs. Perhaps redesign this?
            cyphesis-rulessimulation_python
    )
    target_compile_options(${TEST_NAME} PUBLIC "-w")
    add_dependencies(check ${TEST_NAME})

    #If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
    set_property(TARGET ${TEST_NAME} PROPERTY ENABLE_EXPORTS 1)
    #If we're running with AddressSanitizer we should disable One Definition Rule violations, since the way we do stubs violates this.
    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT ASAN_OPTIONS=detect_odr_violation=0)
endmacro()

macro(wf_add_benchmark TEST_FILE)
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ../src/common/debug.cpp TestWorld.cpp ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_dependencies(benchmark ${TEST_NAME})

    #If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
    set_property(TARGET ${TEST_NAME} PROPERTY ENABLE_EXPORTS 1)

endmacro()

#### Common tests #####
wf_add_test(TestBaseTest.cpp)
wf_add_test(modules/RefTest.cpp)

wf_add_test(common/OperationsDispatcherTest.cpp)
wf_add_test(common/logTest.cpp ../src/common/log.cpp)
#wf_add_test(common/InheritanceTest.cpp ../src/common/Inheritance.cpp ../src/common/custom.cpp)
wf_add_test(rules/simulation/PropertyTest.cpp ../src/common/Property.cpp ../src/rules/simulation/EntityProperties.cpp ../src/common/type_utils.cpp ../src/common/PropertyUtil.cpp)
wf_add_test(common/systemTest.cpp ../src/common/system.cpp)
wf_add_test(common/system_netTest.cpp ../src/common/system_net.cpp)
wf_add_test(common/system_uidTest.cpp ../src/common/system_uid.cpp)
wf_add_test(common/type_utilsTest.cpp ../src/common/type_utils.cpp)
wf_add_test(common/constTest.cpp ../src/common/const.cpp)
wf_add_test(common/randomTest.cpp)
wf_add_test(common/ConnectTest.cpp)
wf_add_test(common/MonitorTest.cpp)
wf_add_test(common/SetupTest.cpp)
wf_add_test(common/TickTest.cpp)
wf_add_test(common/UpdateTest.cpp)
wf_add_test(common/AtlasFileLoaderTest.cpp ../src/common/AtlasFileLoader.cpp)
wf_add_test(server/BaseWorldTest.cpp ../src/rules/simulation/BaseWorld.cpp)
wf_add_test(common/idTest.cpp ../src/common/id.cpp)
wf_add_test(common/StorageTest.cpp ../src/common/Storage.cpp)
wf_add_test(common/debugTest.cpp ../src/common/debug.cpp)
wf_add_test(common/globalsTest.cpp ../src/common/globals.cpp)
target_compile_definitions(globalsTest PUBLIC -DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}" -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}" -DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}" -DLOCALSTATEDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")
wf_add_test(common/OperationRouterTest.cpp)
wf_add_test(common/RouterTest.cpp ../src/common/Router.cpp)
wf_add_test(common/client_socketTest.cpp ../src/common/client_socket.cpp)
wf_add_test(common/customTest.cpp ../src/common/custom.cpp)
wf_add_test(common/MonitorsTest.cpp ../src/common/Monitors.cpp ../src/common/Variable.cpp)
wf_add_test(common/newidTest.cpp ../src/common/newid.cpp)
wf_add_test(common/TypeNodeTest.cpp ../src/common/Property.cpp ../src/common/PropertyUtil.cpp)
wf_add_test(common/FormattedXMLWriterTest.cpp ../src/common/FormattedXMLWriter.cpp)
wf_add_test(common/PropertyFactoryTest.cpp ../src/common/Property.cpp ../src/common/PropertyUtil.cpp)
wf_add_test(common/PropertyManagerTest.cpp)
wf_add_test(common/VariableTest.cpp ../src/common/Variable.cpp)
wf_add_test(common/AtlasStreamClientTest.cpp ../src/common/AtlasStreamClient.cpp)
wf_add_test(common/ClientTaskTest.cpp ../src/common/ClientTask.cpp)
wf_add_test(common/utilsTest.cpp ../src/common/utils.cpp)
wf_add_test(common/ShakerTest.cpp ../src/common/Shaker.cpp)
wf_add_test(common/ScriptKitTest.cpp)
wf_add_test(rules/EntityKitTest.cpp)
wf_add_test(common/LinkTest.cpp ../src/common/Link.cpp)
wf_add_test(common/CommSocketTest.cpp)
wf_add_test(common/FileSystemObserverIntegrationTest.cpp ../src/common/FileSystemObserver.cpp)

# PHYSICS_TESTS
wf_add_test(physics/BBoxTest.cpp ../src/physics/BBox.cpp ../src/common/const.cpp)
wf_add_test(physics/Vector3DTest.cpp ../src/physics/Vector3D.cpp)
wf_add_test(physics/QuaternionTest.cpp ../src/physics/Quaternion.cpp)
wf_add_test(physics/transformTest.cpp)
wf_add_test(physics/distanceTest.cpp ../src/physics/Vector3D.cpp)
wf_add_test(physics/ShapeTest.cpp ../src/physics/Shape.cpp ../src/physics/Course.cpp)
wf_add_test(physics/CourseTest.cpp ../src/physics/Course.cpp)

# MODULE_TESTS

wf_add_test(modules/DateTimeTest.cpp ../src/modules/DateTime.cpp)
wf_add_test(rules/LocationTest.cpp)
wf_add_test(rules/EntityRefTest.cpp)
wf_add_test(modules/TerrainContextTest.cpp)

# RULESETS_TESTS

set(ENTITYEXERCISE TestPropertyManager.cpp IGEntityExerciser.cpp common/EntityExerciser.cpp ../src/common/PropertyUtil.cpp)

wf_add_test(rules/OgreMeshDeserializerTest.cpp ../src/rules/simulation/OgreMeshDeserializer.cpp)
wf_add_test(rules/ModifierTest.cpp ../src/rules/Modifier.cpp)
wf_add_test(rules/LocatedEntityTest.cpp common/EntityExerciser.cpp ../src/rules/simulation/LocatedEntity.cpp ../src/rules/simulation/AtlasProperties TestPropertyManager.cpp)
wf_add_test(rules/EntityTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/Entity.cpp ../src/rules/simulation/LocatedEntity.cpp)
wf_add_test(rules/ThingTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/Thing.cpp)
wf_add_test(rules/WorldTest.cpp ${ENTITYEXERCISE} ../src/rules/simulation/World.cpp)
wf_add_test(rules/ThingupdatePropertiesTest.cpp ../src/rules/simulation/Thing.cpp ../src/common/Property.cpp ../src/common/PropertyUtil.cpp)
wf_add_test(rules/TaskTest.cpp ../src/rules/simulation/Task.cpp)
wf_add_test(rules/simulation/EntityPropertyTest.cpp ../src/rules/simulation/EntityProperty.cpp ../src/common/Property.cpp ../src/common/PropertyUtil.cpp)
wf_add_test(rules/simulation/AllPropertyTest.cpp PropertyExerciser.cpp ../src/rules/simulation/AreaProperty.cpp
        ../src/rules/simulation/AtlasProperties.cpp
        ../src/rules/simulation/CalendarProperty.cpp
        ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/LineProperty.cpp
        ../src/rules/simulation/TerrainProperty.cpp
        ../src/rules/simulation/TerrainEffectorProperty.cpp
        ../src/modules/DateTime.cpp
        ../src/common/Property.cpp
        ../src/common/PropertyUtil.cpp
        ../src/rules/simulation/Inheritance.cpp
        ../src/common/custom.cpp
        ../src/common/operations.cpp
        ../src/physics/Shape.cpp
        ../src/physics/Course.cpp)
wf_add_test(rules/ScriptTest.cpp)
wf_add_test(rules/simulation/AreaPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/AreaProperty.cpp
        ../src/common/Property.cpp
        ../src/common/PropertyUtil.cpp)
wf_add_test(rules/BBoxPropertyTest.cpp PropertyCoverage.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/simulation/CalendarPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/CalendarProperty.cpp
        ../src/modules/DateTime.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/simulation/LinePropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/LineProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(server/MindPropertyTest.cpp PropertyCoverage.cpp ../src/server/MindProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)

wf_add_test(rules/ModePropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/ModeProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/SolidPropertyTest.cpp PropertyCoverage.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/StatusPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/StatusProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/SuspendedPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SuspendedProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/TerrainModPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TerrainModProperty.cpp
        ../src/rules/simulation/TerrainProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/PythonClassTest.cpp python_testers.cpp ../src/pythonbase/PythonClass.cpp ../src/pythonbase/PythonMalloc.cpp)
wf_add_test(rules/TerrainPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TerrainProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/TransientPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TransientProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/TasksPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/TasksProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(common/EntityPropertiesTest.cpp
        ../src/rules/simulation/EntityProperties.cpp
        PropertyCoverage.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/AtlasPropertiesTest.cpp PropertyCoverage.cpp ../src/rules/simulation/AtlasProperties.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/SpawnPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/SpawnProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(rules/VisibilityPropertyTest.cpp PropertyCoverage.cpp ../src/rules/simulation/VisibilityProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)

wf_add_test(rules/ai/BaseMindTest.cpp ../src/rules/ai/BaseMind.cpp ../src/rules/ai/MemMap.cpp)
wf_add_test(rules/MemEntityTest.cpp ../src/rules/ai/MemEntity.cpp)
wf_add_test(rules/ai/MemMapTest.cpp ../src/rules/ai/MemMap.cpp ../src/rules/ai/MemEntity.cpp)
wf_add_test(rules/MovementTest.cpp ../src/rules/simulation/Movement.cpp)
wf_add_test(server/ExternalMindTest.cpp ../src/rules/simulation/ExternalMind.cpp)
wf_add_test(rules/PythonContextTest.cpp ../src/pythonbase/PythonContext.cpp)

wf_add_test(rules/TerrainModTest.cpp ../src/rules/simulation/TerrainModTranslator.cpp)
wf_add_test(rules/TerrainEffectorPropertyTest.cpp ../src/rules/simulation/TerrainEffectorProperty.cpp)
wf_add_test(rules/simulation/GeometryPropertyTest.cpp
        PropertyCoverage.cpp
        ../src/rules/simulation/GeometryProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)

#Python ruleset tests

wf_add_test(rules/Python_APITest.cpp python_testers.cpp)

wf_add_test(rules/Py_QuaternionTest.cpp python_testers.cpp)

wf_add_test(rules/Py_Vector3DTest.cpp python_testers.cpp)

wf_add_test(rules/Py_Point3DTest.cpp python_testers.cpp)

wf_add_test(rules/Py_BBoxTest.cpp python_testers.cpp)

wf_add_test(rules/Py_LocationTest.cpp python_testers.cpp)

wf_add_test(rules/Py_RootEntityTest.cpp python_testers.cpp)

wf_add_test(rules/Py_OperationTest.cpp python_testers.cpp)

wf_add_test(rules/Py_OplistTest.cpp python_testers.cpp)

wf_add_test(rules/Py_ThingTest.cpp python_testers.cpp TestPropertyManager.cpp)

wf_add_test(rules/ai/Py_MapTest.cpp python_testers.cpp TestPropertyManager.cpp)

wf_add_test(rules/Py_TaskTest.cpp python_testers.cpp)

wf_add_test(rules/Py_WorldTest.cpp python_testers.cpp)

wf_add_test(rules/Py_MessageTest.cpp python_testers.cpp)

wf_add_test(rules/Py_PropertyTest.cpp python_testers.cpp)

wf_add_test(rules/Py_TerrainPropertyTest.cpp python_testers.cpp)

#wf_add_test(Py_TerrainModPropertyTest.cpp python_testers.cpp)

#wf_add_test(Py_ShapeTest.cpp python_testers.cpp)

wf_add_test(rules/Py_FilterTest.cpp python_testers.cpp)

wf_add_test(rules/PythonWrapperTest.cpp python_testers.cpp)

#Entity filter tests

wf_add_test(rules/entityfilter/EntityFilterTest.cpp
        ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/rules/simulation/ModeDataProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
)
target_compile_definitions(EntityFilterTest PRIVATE -DBOOST_SPIRIT_DEBUG)

wf_add_test(rules/entityfilter/EntityFilterParserTest.cpp
        ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
)

wf_add_test(rules/entityfilter/EntityFilterProvidersTest.cpp ../src/rules/simulation/EntityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/rules/simulation/ModeDataProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
)


# RULESETS_INTEGRATION

wf_add_test(rules/simulation/GeometryPropertyIntegration.cpp ../src/rules/simulation/GeometryProperty.cpp)

wf_add_benchmark(server/PhysicalDomainBenchmark.cpp ../src/rules/simulation/PhysicalDomain.cpp)

wf_add_test(server/PhysicalDomainIntegrationTest.cpp ../src/rules/simulation/PhysicalDomain.cpp)

wf_add_test(rules/PropertyEntityIntegration.cpp ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
        ../src/rules/Modifier.cpp
)
wf_add_test(rules/TerrainModPropertyIntegration.cpp ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/rules/simulation/TerrainEffectorProperty.cpp
        ../src/rules/simulation/TerrainModProperty.cpp
        ../src/rules/simulation/TerrainModTranslator.cpp
        ../src/rules/simulation/TerrainProperty.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
)
wf_add_test(rules/simulation/ContainedVisibilityPropertyIntegration.cpp ../src/rules/simulation/ContainedVisibilityProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/common/PropertyUtil.cpp
)
wf_add_test(rules/ai/BaseMindMapEntityIntegration.cpp
        ../src/rules/ai/BaseMind.cpp
        ../src/rules/ai/MemEntity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/rules/ai/MemMap.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
        ../src/client/ClientPropertyManager.cpp
)
wf_add_test(rules/simulation/AreaPropertyIntegration.cpp ../src/rules/simulation/AreaProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/common/PropertyUtil.cpp
        ../src/physics/Course.cpp
        ../src/physics/Shape.cpp
)
wf_add_test(rules/SuspendedPropertyIntegration.cpp ../src/rules/simulation/SuspendedProperty.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/common/PropertyUtil.cpp
)
wf_add_test(rules/ThingIntegration.cpp)
wf_add_test(rules/RulesIntegrationTest.cpp)
wf_add_test(rules/ObserverIntegrationTest.cpp)
wf_add_test(rules/TerrainPropertyIntegration.cpp)


# CLIENT_TESTS
wf_add_test(client/Py_CreatorClientTest.cpp python_testers.cpp ../src/client/cyclient/CyPy_CreatorClient.cpp
        ../src/client/cyclient/CyPy_ObserverClient.cpp
        ../src/client/cyclient/Python_ClientAPI.cpp)

wf_add_test(client/Py_ObserverClientTest.cpp python_testers.cpp ../src/client/cyclient/CyPy_CreatorClient.cpp
        ../src/client/cyclient/CyPy_ObserverClient.cpp
        ../src/client/cyclient/Python_ClientAPI.cpp)

wf_add_test(client/ClientConnectionTest.cpp ../src/client/cyclient/ClientConnection.cpp)
wf_add_test(client/BaseClientTest.cpp ../src/client/cyclient/BaseClientLegacy.cpp)
wf_add_test(client/ClientPropertyManagerTest.cpp ../src/client/ClientPropertyManager.cpp)


# CLIENT_INTEGRATION_TESTS
wf_add_test(client/ClientConnectionIntegration.cpp ../src/client/cyclient/ClientConnection.cpp
        ../src/common/AtlasStreamClient.cpp)

# SERVER_TESTS

wf_add_test(server/ArchetypeFactoryIntegrationTest.cpp
        ../src/server/ArchetypeFactory.cpp
        ../src/server/ArchetypeRuleHandler.cpp
        ../src/server/EntityBuilder.cpp
        ../src/server/EntityFactory.cpp
        ../src/server/EntityRuleHandler.cpp
        ../src/rules/simulation/CorePropertyManager.cpp)

wf_add_test(server/RulesetTest.cpp ../src/server/Ruleset.cpp)
wf_add_test(server/EntityBuilderTest.cpp ../src/server/EntityBuilder.cpp)
wf_add_test(common/PropertyFlagTest.cpp
        TestPropertyManager.cpp
        ../src/rules/simulation/Thing.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp)
wf_add_test(server/AccountTest.cpp
        ../src/server/Account.cpp
        ../src/common/debug.cpp)
wf_add_test(server/AdminTest.cpp
        ../src/server/Admin.cpp
        ../src/common/debug.cpp)
wf_add_test(server/ServerAccountTest.cpp
        ../src/server/ServerAccount.cpp)
wf_add_test(server/PlayerTest.cpp
        ../src/server/Player.cpp
        ../src/common/debug.cpp)
wf_add_test(server/EntityFactoryTest.cpp
        ../src/server/EntityFactory.cpp
        ../src/rules/Modifier.cpp
        ../src/common/PropertyUtil.cpp
)
wf_add_test(common/buildidTest.cpp ${PROJECT_BINARY_DIR}/src/server/buildid.cpp)
wf_add_test(server/ConnectionTest.cpp ../src/server/Connection.cpp)
wf_add_test(server/TrustedConnectionTest.cpp ../src/server/TrustedConnection.cpp)
wf_add_test(server/WorldRouterTest.cpp ../src/rules/simulation/WorldRouter.cpp)
wf_add_test(server/PeerTest.cpp ../src/server/Peer.cpp)
wf_add_test(server/LobbyTest.cpp ../src/server/Lobby.cpp)


wf_add_test(server/ServerRoutingTest.cpp ../src/server/ServerRouting.cpp)
wf_add_test(server/StorageManagerTest.cpp ../src/server/StorageManager.cpp)
wf_add_test(server/HttpHandlingTest.cpp ../src/common/net/HttpHandling.cpp)

# SERVER_COMM_TESTS
wf_add_test(server/CommPeerTest.cpp ../src/server/CommPeer.cpp)
wf_add_test(server/CommMDNSPublisherTest.cpp ../src/server/CommMDNSPublisher.cpp)
wf_add_test(server/TeleportAuthenticatorTest.cpp ../src/server/PossessionAuthenticator.cpp)
wf_add_test(server/TeleportStateTest.cpp ../src/server/TeleportState.cpp)
wf_add_test(server/PendingTeleportTest.cpp ../src/server/PendingPossession.cpp)
wf_add_test(server/JunctureTest.cpp ../src/server/Juncture.cpp)
wf_add_test(server/ConnectableRouterTest.cpp)
wf_add_test(server/OpRuleHandlerTest.cpp ../src/server/OpRuleHandler.cpp)
wf_add_test(server/EntityRuleHandlerTest.cpp ../src/server/EntityRuleHandler.cpp)

wf_add_test(server/PropertyRuleHandlerTest.cpp ../src/server/PropertyRuleHandler.cpp)
wf_add_test(server/IdleConnectorTest.cpp ../src/server/IdleConnector.cpp)
wf_add_test(server/PersistenceTest.cpp ../src/server/Persistence.cpp)
wf_add_test(server/SystemAccountTest.cpp ../src/server/SystemAccount.cpp)
wf_add_test(server/CorePropertyManagerTest.cpp ../src/rules/simulation/CorePropertyManager.cpp)

# SERVER_INTEGRATION_TESTS

wf_add_test(server/WorldRouterIntegration.cpp ../src/rules/simulation/WorldRouter.cpp
        ../src/server/EntityBuilder.cpp
        ../src/server/EntityFactory.cpp
        ../src/server/EntityRuleHandler.cpp
        ../src/rules/simulation/Domain.cpp
        ../src/rules/simulation/LocatedEntity.cpp
        ../src/rules/simulation/Entity.cpp
        ../src/rules/simulation/Thing.cpp
        ../src/rules/simulation/World.cpp
        ../src/rules/simulation/BaseWorld.cpp
        ../src/rules/Modifier.cpp
)
wf_add_test(server/RulesetIntegration.cpp ../src/server/Ruleset.cpp
        ../src/server/EntityBuilder.cpp
        ../src/server/EntityFactory.cpp
        ../src/server/OpRuleHandler.cpp
        ../src/server/EntityRuleHandler.cpp
        ../src/server/ArchetypeRuleHandler.cpp
        ../src/server/PropertyRuleHandler.cpp
        ../src/server/Persistence.cpp
        ../src/rules/simulation/BaseWorld.cpp
        ../src/rules/Modifier.cpp
)
#Rulesetintegration_LDFLAGS = $(PYTHON_LINKER_FLAGS)
wf_add_test(server/ConnectionShakerIntegration.cpp ../src/server/Connection.cpp
        ../src/common/Shaker.cpp)
wf_add_test(server/ConnectionCreatorIntegration.cpp
        ../src/server/Connection.cpp)
wf_add_test(server/TrustedConnectionCreatorIntegration.cpp
        ../src/server/Connection.cpp)
wf_add_test(server/AccountConnectionCharacterIntegration.cpp
        ../src/server/Account.cpp
        ../src/server/Connection.cpp)
wf_add_test(server/AccountIntegration.cpp)

wf_add_test(server/EntityFactoryTypeNodeIntegration.cpp TestPropertyManager.cpp ../src/server/EntityFactory.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
        ../src/rules/Modifier.cpp)
wf_add_test(server/AccountConnectionIntegration.cpp ../src/server/Account.cpp
        ../src/server/Admin.cpp
        ../src/server/Connection.cpp
        ../src/server/Lobby.cpp
        ../src/server/Player.cpp
        ../src/server/ServerAccount.cpp
        ../src/server/ServerRouting.cpp
        ../src/server/SystemAccount.cpp
        ../src/rules/simulation/BaseWorld.cpp
        ../src/rules/simulation/Inheritance.cpp
        ../src/common/PropertyUtil.cpp
        ../src/common/Property.cpp
        ../src/common/Router.cpp
        ../src/common/custom.cpp
        ../src/common/operations.cpp)
wf_add_test(server/AccountServerLobbyIntegration.cpp ../src/server/Account.cpp
        ../src/server/ServerRouting.cpp
        ../src/server/Lobby.cpp)


# Other TESTS
#wf_add_test(MasterTest.cpp ${PROJECT_SOURCE_DIR}/server/Master.cpp)


# TOOLS_TESTS
wf_add_test(tools/AdminClientTest.cpp ../src/tools/AdminClient.cpp)
wf_add_test(tools/FlusherTest.cpp ../src/tools/Flusher.cpp
        ../src/common/ClientTask.cpp)
wf_add_test(tools/OperationMonitorTest.cpp ../src/tools/OperationMonitor.cpp
        ../src/common/ClientTask.cpp)
wf_add_test(tools/EntityExporterTest.cpp ../src/tools/EntityExporterBase.cpp)


# PYTHON_TESTS
#wf_add_test(python_class.cpp)

wf_add_test(navigation/SteeringIntegration.cpp)

