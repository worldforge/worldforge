INCLUDES = -I$(top_srcdir)/src  -I$(top_builddir)/src -DPREFIX=\"@prefix@\"

bin_PROGRAMS = metaserver-ng testserver testclient

metaserver_ng_SOURCES = main.cpp MetaServer.cpp MetaServerHandler.cpp MetaServerHandlerTCP.cpp MetaServerHandlerUDP.cpp MetaServerPacket.cpp DataObject.cpp PacketLogger.cpp
testserver_SOURCES = TestServer.cpp MetaServerPacket.cpp PacketLogger.cpp
testclient_SOURCES = TestClient.cpp MetaServerPacket.cpp PacketLogger.cpp

noinst_HEADERS = MetaServerHandler.hpp MetaServerHandlerTCP.hpp MetaServerHandlerUDP.hpp MetaServer.hpp MetaServerPacket.hpp MetaServerProtocol.hpp TestServer.hpp DataObject.hpp PacketLogger.hpp

metaserver_ng_LDFLAGS = -Wl

TESTS = \
    MetaServerPacket_unittest \
    MetaServer_unittest \
    DataObject_unittest \
	MetaServerHandlerTCP_unittest \
	MetaServerHandlerUDP_unittest \
	PacketLogger_unittest

check_PROGRAMS = $(TESTS)

MetaServer_unittest_SOURCES = MetaServer_unittest.cpp
MetaServer_unittest_LDADD = MetaServer.o DataObject.o PacketLogger.o $(CPPUNIT_LIBS)

MetaServerPacket_unittest_SOURCES = MetaServerPacket_unittest.cpp
MetaServerPacket_unittest_LDADD = MetaServerPacket.o $(CPPUNIT_LIBS)

DataObject_unittest_SOURCES = DataObject_unittest.cpp
DataObject_unittest_LDADD = DataObject.o $(CPPUNIT_LIBS)

MetaServerHandlerTCP_unittest_SOURCES = MetaServerHandlerTCP_unittest.cpp
MetaServerHandlerTCP_unittest_LDADD = MetaServerHandlerTCP.o $(CPPUNIT_LIBS)

MetaServerHandlerUDP_unittest_SOURCES = MetaServerHandlerUDP_unittest.cpp
MetaServerHandlerUDP_unittest_LDADD = MetaServerHandlerUDP.o MetaServerPacket.o $(CPPUNIT_LIBS)

PacketLogger_unittest_SOURCES = PacketLogger_unittest.cpp
PacketLogger_unittest_LDADD = PacketLogger.o MetaServerPacket.o $(CPPUNIT_LIBS)

