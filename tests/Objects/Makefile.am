
CPPFLAGS = -I$(top_srcdir)/Atlas

LDADD  = -L$(top_builddir)/Atlas/ -L$(top_builddir)/Atlas/Codecs/ \
         -L$(top_builddir)/Atlas/Filters/ \
         -L$(top_builddir)/Atlas/Message/ -L$(top_builddir)/Atlas/Net/ \
         -L$(top_builddir)/Atlas/Objects/

LIBS = -lAtlasObjects-0.6 -lAtlasNet-0.6 -lAtlas-0.6 -lAtlasCodecs-0.6 \
       -lAtlasFilters-0.6 -lAtlasMessage-0.6 -lAtlas-0.6 $(OTHER_LIBS)

check_PROGRAMS = objects1 objects2 encoder1 decoder1

TESTS = $(check_PROGRAMS)

objects1_SOURCES = objects1.cpp

objects2_SOURCES = objects2.cpp

encoder1_SOURCES = encoder1.cpp

decoder1_SOURCES = decoder1.cpp
