check_PROGRAMS = objects1 objects2 encoder1 decoder1
#check_PROGRAMS = objects2
#check_PROGRAMS = encoder1

TESTS = $(check_PROGRAMS)

CPPFLAGS = -I$(prefix)/include -Wall

objects1_SOURCES = objects1.cpp
objects2_SOURCES = objects2.cpp
encoder1_SOURCES = encoder1.cpp
decoder1_SOURCES = decoder1.cpp

LDADD = \
	-lAtlasCodecs \
        -lAtlasObjects \
        -lAtlasMessage \
        -lAtlas \
	-lbz2

