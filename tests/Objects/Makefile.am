bin_PROGRAMS = objects1 encoder1 decoder1

TESTS = $(bin_PROGRAMS)

CXXFLAGS = -I/usr/local/include
objects1_SOURCES = objects1.cc
encoder1_SOURCES = encoder1.cc
decoder1_SOURCES = decoder1.cc

LDADD = -L/usr/local/lib -lAtlasObjects -lAtlasObjectsEntity \
                 -lAtlasObjectsOperation -lAtlas -lAtlasMessage
