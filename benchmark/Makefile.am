check_PROGRAMS = Objects3_Move Call_Move Message_Move \
		  Static_Move SmartPtr_Move

TESTS = $(check_PROGRAMS)

CPPFLAGS = -I$(prefix)/include -Wall

LDADD  = -L$(prefix)/lib -lsocket++
LDADD += \
        -lAtlasObjects \
        -lAtlasMessage \
	-lAtlas \
	-lAtlasCodecs \
	-lAtlasFilters \
	-lAtlasNet \
	-lbz2


# In a real program the above lines should be:
# LDADD +=-lAtlasCodecs -lAtlasFilters -lAtlasNet

Message_Move_SOURCES = Message_Move.cpp
#Objects_Move_SOURCES = Objects_Move.cc
#Objects2_Move_SOURCES = Objects2_Move.cpp
Objects3_Move_SOURCES = Objects3_Move.cpp
Call_Move_SOURCES = Call_Move.cpp
Static_Move_SOURCES = Static_Move.cpp
SmartPtr_Move_SOURCES = SmartPtr_Move.cpp

#EXTRA_DIST = \
#	benchmark.txt
