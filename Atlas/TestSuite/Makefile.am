
bin_PROGRAMS = test-object-map test-object-list

CXXFLAGS = -Wall -D_ADebug_

test_object_map_SOURCES		= test-object-map.cpp
test_object_mapincludedir	= $(includedir)/Atlas/src/TestSuite
test_object_map_LDADD		= ../Object/.libs/libAtlasObject.a -ldl

test_object_list_SOURCES	= test-object-list.cpp
test_object_listincludedir	= $(includedir)/Atlas/src/TestSuite
test_object_list_LDADD		= ../Object/.libs/libAtlasObject.a -ldl

TESTS = test-object-map test-object-list