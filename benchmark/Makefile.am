
check_PROGRAMS = Objects3_Move Call_Move Message_Move \
		  Static_Move SmartPtr_Move

TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)

LDADD  = -L$(top_builddir)/Atlas/ -L$(top_builddir)/Atlas/Codecs/ \
         -L$(top_builddir)/Atlas/Filters/ -L$(top_builddir)/Atlas/Message/ \
         -L$(top_builddir)/Atlas/Net/ -L$(top_builddir)/Atlas/Objects/

LIBS = -lAtlasObjects-0.6 -lAtlasNet-0.6 -lAtlas-0.6 -lAtlasCodecs-0.6 \
       -lAtlasFilters-0.6 -lAtlasMessage-0.6 -lAtlas-0.6 $(OTHER_LIBS)

Message_Move_SOURCES = Message_Move.cpp

#Objects_Move_SOURCES = Objects_Move.cc

#Objects2_Move_SOURCES = Objects2_Move.cpp

Objects3_Move_SOURCES = Objects3_Move.cpp

Call_Move_SOURCES = Call_Move.cpp

Static_Move_SOURCES = Static_Move.cpp

SmartPtr_Move_SOURCES = SmartPtr_Move.cpp
