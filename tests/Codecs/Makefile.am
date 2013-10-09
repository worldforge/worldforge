
AM_CPPFLAGS = -I$(top_srcdir)

LDADD  = -L$(top_builddir)/Atlas/ -L$(top_builddir)/Atlas/Codecs/ \
         -L$(top_builddir)/Atlas/Filters/ \
         -L$(top_builddir)/Atlas/Message/ -L$(top_builddir)/Atlas/Net/ \
         -L$(top_builddir)/Atlas/Objects/

LIBS = -lAtlasObjects-0.6 -lAtlasNet-0.6 -lAtlas-0.6 -lAtlasCodecs-0.6 \
       -lAtlasFilters-0.6 -lAtlasMessage-0.6 -lAtlas-0.6 $(OTHER_LIBS)

check_PROGRAMS = codecs

TESTS = $(check_PROGRAMS)

codecs_SOURCES = codecs.cpp

