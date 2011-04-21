INCLUDES = -I$(top_srcdir)/src  -I$(top_builddir)/src -DPREFIX=\"@prefix@\"

bin_PROGRAMS = metaserver-ng

metaserver_ng_SOURCES = MetaServer.cpp MetaServerHandlerTCP.cpp

metaserver_ng_LDFLAGS = -Wl

noinst_HEADERS = MetaServer.hpp MetaServerHandlerTCP.hpp TestServer.hpp
