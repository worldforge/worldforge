noinst_PROGRAMS = simple_client simple_server

CPPFLAGS = -I$(prefix)/include

LDADD  = -L$(prefix)/lib -lsocket++
LDADD += \
	-lAtlasCodecs \
	-lAtlasFilters \
	-lAtlas \
	-lAtlasNet \
	-lbz2

# In a real program the above lines should be:
# LDADD +=-lAtlasCodecs -lAtlasFilters -lAtlasNet

simple_client_SOURCES = simple_client.cpp
simple_server_SOURCES = simple_server.cpp

EXTRA_DIST = \
	DebugBridge.h building.dox index.dox message.dox \
	objects.dox preamble.dox simple.dox
