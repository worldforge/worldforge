bin_PROGRAMS = simple_client simple_server

CPPFLAGS = -I$(prefix)/include

LDADD  = -L$(prefix)/lib -lsocket++
LDADD += -lAtlas -lAtlasCodecs -lAtlasFilters -lAtlasNet

simple_client_SOURCES = simple_client.cc
simple_server_SOURCES = simple_server.cc

EXTRA_DIST = \
	DebugBridge.h building.dox index.dox message.dox \
	objects.dox preamble.dox simple.dox
