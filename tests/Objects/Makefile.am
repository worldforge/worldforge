check_PROGRAMS = objects1 objects2 encoder1 decoder1
#check_PROGRAMS = objects2
#check_PROGRAMS = encoder1

CPPFLAGS = -I../../src -Wall

LDADD  = -L../../src/.libs -L../../src/Codecs/.libs -L../../src/Filters/.libs \
         -L../../src/Message/.libs -L../../src/Net/.libs \
         -L../../src/Objects/.libs

LIBS = -lAtlasObjects -lAtlasNet -lAtlas -lAtlasCodecs -lAtlasFilters \
       -lAtlasMessage -lAtlas -lbz2 -lz

LDFLAGS = -all-static

TESTS = $(check_PROGRAMS)

objects1_SOURCES = objects1.cpp
objects2_SOURCES = objects2.cpp
encoder1_SOURCES = encoder1.cpp
decoder1_SOURCES = decoder1.cpp
