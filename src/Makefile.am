INCLUDES = -I$(top_srcdir)/src  -I$(top_builddir)/src -DPREFIX=\"@prefix@\"

bin_PROGRAMS = metaserver-ng

metaserver_ng_SOURCES = main.cpp MetaServer.cpp MetaServerHandlerTCP.cpp MetaServerHandlerUDP.cpp MetaServerPacket.cpp

metaserver_ng_LDFLAGS = -Wl

noinst_HEADERS = MetaServer.hpp MetaServerHandlerTCP.hpp MetaServerHandlerUDP.hpp TestServer.hpp
