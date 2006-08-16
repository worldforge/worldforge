INCLUDES = -I$(top_builddir) -I$(top_srcdir)

check_PROGRAMS = testEris
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

testEris_LDADD = $(top_builddir)/Eris/liberis-1.3.la

noinst_PROGRAMS  = metaquery

metaquery_SOURCES = metaQuery.cpp
metaquery_LDADD = $(top_builddir)/Eris/liberis-1.3.la
