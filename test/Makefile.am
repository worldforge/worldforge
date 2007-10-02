INCLUDES = -I$(top_builddir) -I$(top_srcdir)

check_PROGRAMS = testEris Entity_unittest Account_unittest Alarm_unittest \
                 Avatar_unittest BaseConnection_unittest Calendar_unittest \
                 Connection_unittest DeleteLater_unittest Entity_unittest \
                 EntityRef_unittest EntityRouter_unittest Exceptions_unittest \
                 Factory_unittest IGRouter_unittest Lobby_unittest \
                 Log_unittest LogStream_unittest MetaQuery_unittest \
                 Metaserver_unittest Operations_unittest Person_unittest \
                 Poll_unittest PollDefault_unittest Redispatch_unittest \
                 Response_unittest Room_unittest Router_unittest \
                 ServerInfo_unittest Task_unittest TimedEventService_unittest \
                 Timeout_unittest TypeBoundRedispatch_unittest \
                 TypeInfo_unittest TypeService_unittest Types_unittest \
                 UIFactory_unittest View_unittest
TESTS = $(check_PROGRAMS)

testEris_SOURCES = tests.cpp \
    stubServer.h stubServer.cpp \
    clientConnection.cpp clientConnection.h \
    agent.cpp agent.h \
    commander.cpp commander.h \
    controller.cpp controller.h \
    testUtils.h testUtils.cpp \
    objectSummary.h \
    netTests.cpp netTests.h \
    setupHelpers.cpp setupHelpers.h \
    signalHelpers.h \
    testOutOfGame.cpp testOutOfGame.h \
    viewTest.cpp viewTest.h \
    avatarTest.cpp avatarTest.h \
    calendarTest.cpp calendarTest.h

LDADD = $(top_builddir)/Eris/liberis-1.3.la

noinst_PROGRAMS  = metaquery

metaquery_SOURCES = metaQuery.cpp
metaquery_LDADD = $(top_builddir)/Eris/liberis-1.3.la

