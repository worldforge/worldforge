check_PROGRAMS = objects1 encoder1 decoder1

TESTS = $(check_PROGRAMS)

objects1_SOURCES = objects1.cc
encoder1_SOURCES = encoder1.cc
decoder1_SOURCES = decoder1.cc

LDADD = \
        ../../src/Objects/.libs/libAtlasObjects.a \
        ../../src/Objects/Entity/.libs/libAtlasObjectsEntity.a \
        ../../src/Objects/Operation/.libs/libAtlasObjectsOperation.a \
        ../../src/Message/.libs/libAtlasMessage.a \
        ../../src/.libs/libAtlas.a

