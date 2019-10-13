wf_add_test_linked(Account_integrationtest.cpp)
wf_add_test_linked(Account_unittest.cpp)
wf_add_test_linked(Avatar_unittest.cpp)
wf_add_test_linked(BaseConnection_unittest.cpp)
wf_add_test(Calendar_unittest.cpp
        ../src/Eris/Calendar.cpp)
wf_add_test_linked(Connection_unittest.cpp)
wf_add_test_linked(DeleteLater_unittest.cpp)
wf_add_test_linked(Entity_unittest.cpp)
wf_add_test_linked(EntityRef_unittest.cpp)
wf_add_test_linked(EntityRouter_unittest.cpp)
wf_add_test_linked(EventService_unittest.cpp)
wf_add_test_linked(Exceptions_unittest.cpp)
wf_add_test_linked(Factory_unittest.cpp)
wf_add_test(IGRouter_unittest.cpp ../src/Eris/IGRouter.cpp ../src/Eris/Response.cpp)
wf_add_test_linked(Lobby_unittest.cpp)
wf_add_test_linked(Log_unittest.cpp)
wf_add_test_linked(LogStream_unittest.cpp)
wf_add_test_linked(MetaQuery_unittest.cpp)
wf_add_test_linked(Metaserver_integrationtest.cpp)
wf_add_test_linked(Metaserver_unittest.cpp)
wf_add_test_linked(Operations_unittest.cpp)
wf_add_test_linked(Person_unittest.cpp)
wf_add_test_linked(Poll_unittest.cpp)
wf_add_test_linked(PollWinsock_unittest.cpp)
wf_add_test_linked(Redispatch_unittest.cpp)
wf_add_test_linked(Response_unittest.cpp)
wf_add_test_linked(Room_unittest.cpp)
wf_add_test_linked(Router_unittest.cpp)
wf_add_test_linked(ServerInfo_unittest.cpp)
wf_add_test_linked(Task_unittest.cpp)
wf_add_test_linked(TransferInfo_unittest.cpp)
wf_add_test_linked(TypeBoundRedispatch_unittest.cpp)
wf_add_test_linked(TypeInfo_unittest.cpp)
wf_add_test_linked(Types_unittest.cpp)
wf_add_test_linked(TypeService_unittest.cpp)
wf_add_test_linked(View_unittest.cpp)
wf_add_test_linked(ActiveMarker_UnitTest.cpp ../src/Eris/ActiveMarker.cpp)

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
