check_PROGRAMS = objects1 objects2 encoder1 decoder1
#check_PROGRAMS = objects2
#check_PROGRAMS = encoder1

TESTS = $(check_PROGRAMS)

CPPFLAGS = -I$(prefix)/include -Wall

objects1_SOURCES = objects1.cc
objects2_SOURCES = objects2.cpp
encoder1_SOURCES = encoder1.cc
decoder1_SOURCES = decoder1.cc

LDADD = \
        ../../src/Objects/.libs/libAtlasObjects.a \
        ../../src/Message/.libs/libAtlasMessage.a \
        ../../src/.libs/libAtlas.a

