TESTS = testEris

check_PROGRAMS = $(TESTS)

testEris_SOURCES = tests.cpp \
    testPlayer.cpp testPlayer.h \
    stubServer.h stubServer.cpp \
    testUtils.h testUtils.cpp \
    stubConnection.cpp stubConnection.h \
    testConnection.cpp testConnection.h \
    testEntity.cpp testEntity.h \
    testLobby.cpp testLobby.h

INCLUDES = @CPPUNIT_CFLAGS@

testEris_LDADD = @CPPUNIT_LIBS@ $(top_builddir)/Eris/liberis-1.0.la
