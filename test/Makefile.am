TESTS = testEris

INCLUDES = -I$(top_srcdir)

check_PROGRAMS = $(TESTS)

testEris_SOURCES = tests.cpp \
    stubServer.h stubServer.cpp \
    clientConnection.cpp clientConnection.h \
    agent.cpp agent.h \
    commander.cpp commander.h \
    controller.cpp controller.h \
    testUtils.h \
    objectSummary.h

testEris_LDADD = $(top_builddir)/Eris/liberis-1.3.la

noinst_PROGRAMS  = metaquery

metaquery_SOURCES = metaQuery.cpp
metaquery_LDADD = $(top_builddir)/Eris/liberis-1.3.la
