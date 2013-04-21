
check_PROGRAMS = Objects_asMessage Objects3_Move Call_Move Message_Move \
                 Static_Move Objects_iterator
                 
EXTRA_PROGRAMS = SmartPtr_Move

TESTS = $(check_PROGRAMS)

AM_CPPFLAGS = -I$(top_srcdir)

LDADD  = -L$(top_builddir)/Atlas/ -L$(top_builddir)/Atlas/Codecs/ \
         -L$(top_builddir)/Atlas/Filters/ -L$(top_builddir)/Atlas/Message/ \
         -L$(top_builddir)/Atlas/Net/ -L$(top_builddir)/Atlas/Objects/

LIBS = -lAtlasObjects-0.7 -lAtlasNet-0.7 -lAtlas-0.7 -lAtlasCodecs-0.7 \
       -lAtlasFilters-0.7 -lAtlasMessage-0.7 -lAtlas-0.7 $(OTHER_LIBS)

Message_Move_SOURCES = Message_Move.cpp timer.h

#Objects_Move_SOURCES = Objects_Move.cc timer.h

#Objects2_Move_SOURCES = Objects2_Move.cpp timer.h

Objects3_Move_SOURCES = Objects3_Move.cpp timer.h

Call_Move_SOURCES = Call_Move.cpp timer.h

Static_Move_SOURCES = Static_Move.cpp timer.h

SmartPtr_Move_SOURCES = SmartPtr_Move.cpp timer.h

Objects_asMessage_SOURCES = Objects_asMessage.cpp timer.h
Objects_iterator_SOURCES = Objects_iterator.cpp timer.h
