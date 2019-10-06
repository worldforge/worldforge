wf_add_test(Account_integrationtest.cpp)
wf_add_test(Account_unittest.cpp)
wf_add_test(Avatar_unittest.cpp)
wf_add_test(BaseConnection_unittest.cpp)
wf_add_test(Calendar_unittest.cpp)
wf_add_test(Connection_unittest.cpp)
wf_add_test(DeleteLater_unittest.cpp)
wf_add_test(Entity_unittest.cpp)
wf_add_test(EntityRef_unittest.cpp)
wf_add_test(EntityRouter_unittest.cpp)
wf_add_test(EventService_unittest.cpp)
wf_add_test(Exceptions_unittest.cpp)
wf_add_test(Factory_unittest.cpp)
wf_add_test(IGRouter_unittest.cpp)
wf_add_test(Lobby_unittest.cpp)
wf_add_test(Log_unittest.cpp)
wf_add_test(LogStream_unittest.cpp)
wf_add_test(MetaQuery_unittest.cpp)
wf_add_test(Metaserver_integrationtest.cpp)
wf_add_test(Metaserver_unittest.cpp)
wf_add_test(Operations_unittest.cpp)
wf_add_test(Person_unittest.cpp)
wf_add_test(Poll_unittest.cpp)
wf_add_test(PollWinsock_unittest.cpp)
wf_add_test(Redispatch_unittest.cpp)
wf_add_test(Response_unittest.cpp)
wf_add_test(Room_unittest.cpp)
wf_add_test(Router_unittest.cpp)
wf_add_test(ServerInfo_unittest.cpp)
wf_add_test(Task_unittest.cpp)
wf_add_test(TransferInfo_unittest.cpp)
wf_add_test(TypeBoundRedispatch_unittest.cpp)
wf_add_test(TypeInfo_unittest.cpp)
wf_add_test(Types_unittest.cpp)
wf_add_test(TypeService_unittest.cpp)
wf_add_test(View_unittest.cpp)
wf_add_test(ActiveMarker_UnitTest.cpp)

#wf_add_test(testEris tests.cpp
#        stubServer.h stubServer.cpp
#        clientConnection.cpp clientConnection.h
#        agent.cpp agent.h
#        commander.cpp commander.h
#        controller.cpp controller.h
#        testUtils.h testUtils.cpp
#        objectSummary.h
#        netTests.cpp netTests.h
#        setupHelpers.cpp setupHelpers.h
#        signalHelpers.h
#        testOutOfGame.cpp testOutOfGame.h
#        viewTest.cpp viewTest.h
#        avatarTest.cpp avatarTest.h
#        calendarTest.cpp calendarTest.h)

add_executable(metaQuery metaQuery.cpp)
target_link_libraries(metaQuery ${LIBNAME})
add_executable(eris_connect connect.cpp)
target_link_libraries(eris_connect ${LIBNAME})
