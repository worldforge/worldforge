
check_PROGRAMS = Objects3_Move Call_Move Message_Move \
		  Static_Move SmartPtr_Move

TESTS = $(check_PROGRAMS)

CPPFLAGS = -I$(top_srcdir)/src -Wall

LDADD  = -L$(top_builddir)/src/ -L$(top_builddir)/src/Codecs/ -L$(top_builddir)/src/Filters/ \
         -L$(top_builddir)/src/Message/ -L$(top_builddir)/src/Net/ -L$(top_builddir)/src/Objects/

LIBS = -lAtlasObjects -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasFilters \
       -lAtlasMessage -lAtlas -lbz2 -lz

Message_Move_SOURCES = Message_Move.cpp

#Objects_Move_SOURCES = Objects_Move.cc

#Objects2_Move_SOURCES = Objects2_Move.cpp

Objects3_Move_SOURCES = Objects3_Move.cpp

Call_Move_SOURCES = Call_Move.cpp

Static_Move_SOURCES = Static_Move.cpp

SmartPtr_Move_SOURCES = SmartPtr_Move.cpp
