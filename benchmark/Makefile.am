#noinst_PROGRAMS = Objects2_Move Call_Move \
#		  Static_Move SmartPtr_Move

CPPFLAGS = -I$(prefix)/include -Wall

LDADD  = -L$(prefix)/lib -lsocket++
LDADD += \
        ../src/Objects/.libs/libAtlasObjects.a \
        ../src/Objects/Entity/.libs/libAtlasObjectsEntity.a \
        ../src/Objects/Operation/.libs/libAtlasObjectsOperation.a \
        ../src/Message/.libs/libAtlasMessage.a \
	../src/.libs/libAtlas.a \
	../src/Codecs/.libs/libAtlasCodecs.a \
	../src/Filters/.libs/libAtlasFilters.a \
	../src/Net/.libs/libAtlasNet.a


# In a real program the above lines should be:
# LDADD +=-lAtlasCodecs -lAtlasFilters -lAtlasNet

#Message_Move_SOURCES = Message_Move.cc
#Objects_Move_SOURCES = Objects_Move.cc
#Objects2_Move_SOURCES = Objects2_Move.cpp
#Call_Move_SOURCES = Call_Move.cc
#Static_Move_SOURCES = Static_Move.cc
#SmartPtr_Move_SOURCES = SmartPtr_Move.cc

#EXTRA_DIST = \
#	benchmark.txt
