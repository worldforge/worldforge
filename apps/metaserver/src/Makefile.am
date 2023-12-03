AM_CPPFLAGS = -I$(top_srcdir)/src  -I$(top_builddir)/src -DPREFIX=\"@prefix@\"

#bin_PROGRAMS = metaserver-ng testserver testclient packetplayer pdnspipe
bin_PROGRAMS = testserver testclient pdnspipe
if BUILD_SERVER
bin_PROGRAMS += metaserver-ng
endif

lib_LTLIBRARIES = libmsapi.la
libmsapi_la_SOURCES = MetaServerPacket.hpp MetaServerPacket.hpp MetaServerAPI.hpp
libmsapi_la_LDFLAGS = -version-info @APIVER@

include_HEADERS = MetaServerPacket.hpp MetaServerProtocol.hpp MetaServerAPI.hpp

msapidir = $(prefix)/api/metaserver
msapi_DATA = MetaServerVersion.hpp MetaServerPacket.hpp MetaServerPacket.cpp MetaServerProtocol.hpp MetaServerAPI.hpp ../README.API

noinst_HEADERS = DataObject.hpp MetaServer.hpp MetaServerHandler.hpp MetaServerPacket.hpp \
                 PacketReader.hpp MetaServerHandlerUDP.hpp \
                 MetaServerProtocol.hpp PacketLogger.hpp TestServer.hpp

metaserver_ng_SOURCES = main.cpp MetaServer.cpp MetaServerHandler.cpp \
                        MetaServerHandlerUDP.cpp \
                        MetaServerPacket.cpp DataObject.cpp
                                         
testserver_SOURCES = TestServer.cpp MetaServerPacket.cpp

testclient_SOURCES = TestClient.cpp MetaServerPacket.cpp

pdnspipe_SOURCES = PDNSPipe.cpp MetaServerPacket.cpp

metaserver_ng_LDFLAGS = 

TESTS = \
    MetaServerPacket_unittest \
    MetaServer_unittest \
    DataObject_unittest \
    MetaServerHandlerUDP_unittest 

check_PROGRAMS = $(TESTS)

MetaServer_unittest_SOURCES = MetaServer_unittest.cpp
MetaServer_unittest_LDADD = MetaServer.o MetaServerPacket.o DataObject.o $(CPPUNIT_LIBS)

MetaServerPacket_unittest_SOURCES = MetaServerPacket_unittest.cpp
MetaServerPacket_unittest_LDADD = MetaServerPacket.o $(CPPUNIT_LIBS)

DataObject_unittest_SOURCES = DataObject_unittest.cpp
DataObject_unittest_LDADD = DataObject.o $(CPPUNIT_LIBS)

MetaServerHandlerUDP_unittest_SOURCES = MetaServerHandlerUDP_unittest.cpp
MetaServerHandlerUDP_unittest_LDADD = MetaServerHandlerUDP.o MetaServerPacket.o $(CPPUNIT_LIBS)

#PacketLogger_unittest_SOURCES = PacketLogger_unittest.cpp
#PacketLogger_unittest_LDADD = PacketLogger.o $(CPPUNIT_LIBS)

#PacketReader_unittest_SOURCES = PacketReader_unittest.cpp
#PacketReader_unittest_LDADD = PacketReader.o $(CPPUNIT_LIBS)

#PacketLogging_integration_SOURCES = PacketLogging_integration.cpp
#PacketLogging_integration_LDADD = PacketReader.o PacketLogger.o MetaServerPacket.o $(CPPUNIT_LIBS)
