bin_PROGRAMS = objects1

TESTS = $(bin_PROGRAMS)

CXXFLAGS = -I/usr/local/include
objects1_SOURCES = objects1.cc

objects1_LDADD = -L/usr/local/lib -lAtlasObjects -lAtlasObjectsEntity \
                 -lAtlasObjectsOperation -lAtlas -lAtlasMessage
