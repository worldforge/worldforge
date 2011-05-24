INCLUDES = -I$(top_srcdir)/src  -I$(top_builddir)/src -DPREFIX=\"@prefix@\"

bin_PROGRAMS = metaserver-ng

metaserver_ng_SOURCES = main.cpp MetaServer.cpp MetaServerHandler.cpp MetaServerHandlerTCP.cpp MetaServerHandlerUDP.cpp MetaServerPacket.cpp

metaserver_ng_LDFLAGS = -Wl

noinst_HEADERS = MetaServer.hpp MetaServerHandlerTCP.hpp MetaServerHandlerUDP.hpp TestServer.hpp

TESTS = \
    MetaServerPacket_unittest \
    MetaServer_unittest

check_PROGRAMS = $(TESTS)

MetaServer_unittest_SOURCES = MetaServer_unittest.cpp
MetaServer_unittest_LDADD = MetaServer.o $(CPPUNIT_LIBS)

MetaServerPacket_unittest_SOURCES = MetaServerPacket_unittest.cpp
MetaServerPacket_unittest_LDADD = MetaServerPacket.o $(CPPUNIT_LIBS)
