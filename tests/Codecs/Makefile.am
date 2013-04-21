
AM_CPPFLAGS = -I$(top_srcdir)

LDADD  = -L$(top_builddir)/Atlas/ -L$(top_builddir)/Atlas/Codecs/ \
         -L$(top_builddir)/Atlas/Filters/ \
         -L$(top_builddir)/Atlas/Message/ -L$(top_builddir)/Atlas/Net/ \
         -L$(top_builddir)/Atlas/Objects/

LIBS = -lAtlasObjects-0.7 -lAtlasNet-0.7 -lAtlas-0.7 -lAtlasCodecs-0.7 \
       -lAtlasFilters-0.7 -lAtlasMessage-0.7 -lAtlas-0.7 $(OTHER_LIBS)

check_PROGRAMS = codecs

TESTS = $(check_PROGRAMS)

codecs_SOURCES = codecs.cpp

