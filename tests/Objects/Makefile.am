
CPPFLAGS = -I$(top_srcdir)/src -Wall

LDADD  = -L$(top_builddir)/src/ -L$(top_builddir)/src/Codecs/ \
         -L$(top_builddir)/src/Filters/ \
         -L$(top_builddir)/src/Message/ -L$(top_builddir)/src/Net/ \
         -L$(top_builddir)/src/Objects/

LIBS = -lAtlasObjects -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasFilters \
       -lAtlasMessage -lAtlas $(OTHER_LIBS)

check_PROGRAMS = objects1 objects2 encoder1 decoder1

TESTS = $(check_PROGRAMS)

objects1_SOURCES = objects1.cpp

objects2_SOURCES = objects2.cpp

encoder1_SOURCES = encoder1.cpp

decoder1_SOURCES = decoder1.cpp
