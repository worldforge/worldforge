
CPPFLAGS = -I$(top_srcdir)/src -Wall

LDADD  = -L$(top_builddir)/src/.libs -L$(top_builddir)/src/Codecs/.libs -L$(top_builddir)/src/Filters/.libs \
         -L$(top_builddir)/src/Message/.libs -L$(top_builddir)/src/Net/.libs \
         -L$(top_builddir)/src/Objects/.libs

LIBS = -lAtlasObjects -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasFilters \
       -lAtlasMessage -lAtlas -lbz2 -lz

LDFLAGS = -all-static

check_PROGRAMS = objects1 objects2 encoder1 decoder1

TESTS = $(check_PROGRAMS)

objects1_SOURCES = objects1.cpp

objects2_SOURCES = objects2.cpp

encoder1_SOURCES = encoder1.cpp

decoder1_SOURCES = decoder1.cpp
