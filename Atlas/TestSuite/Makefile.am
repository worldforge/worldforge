
noinst_PROGRAMS = \
	test-object-map \
	test-object-list \
	test-negotiate \
	test-packed

CXXFLAGS = -Wall -D_ADebug_

test_object_map_SOURCES		= test-object-map.cpp
test_object_mapincludedir	= $(includedir)/Atlas/src/TestSuite
test_object_map_LDADD		= ../Object/.libs/libAtlasObject.a -ldl

test_object_list_SOURCES	= test-object-list.cpp
test_object_listincludedir	= $(includedir)/Atlas/src/TestSuite
test_object_list_LDADD		= ../Object/.libs/libAtlasObject.a -ldl

test_negotiate_SOURCES		= test-negotiate.cc
test_negotiateincludedir	= $(includedir)/Atlas/src/TestSuite
test_negotiate_LDADD		= ../Stream/.libs/libAtlasStream.so ../Object/.libs/libAtlasObject.a ../Codecs/.libs/libAtlasCodecs.so ../Filters/.libs/libAtlasFilters.so -lz -lbz2

test_packed_SOURCES		= test-packed.cc
test_packed_LDADD		= ../Stream/.libs/libAtlasStream.so ../Object/.libs/libAtlasObject.a ../Codecs/.libs/libAtlasCodecs.so ../Filters/.libs/libAtlasFilters.a

TESTS = test-object-map test-object-list test-negotiate

