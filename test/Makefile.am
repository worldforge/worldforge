TESTS = testEris

INCLUDES = -I$(top_srcdir)

check_PROGRAMS = $(TESTS)

testEris_SOURCES = tests.cpp testHarness.h \
    stubServer.h stubServer.cpp \
    testUtils.h testUtils.cpp \
    testConnection.cpp testConnection.h \
    testPlayer.h testPlayer.cpp

#testEris_LDADD = $(top_builddir)/Eris/liberis-1.2.la

testEris_LDADD = $(top_builddir)/Eris/.libs/liberis-1.2.so


