
noinst_PROGRAMS = \
	test-object-map \
	test-object-list \
	test-negotiate \
	test-packed \
	test-xml

CXXFLAGS = -Wall -g

test_object_map_SOURCES		= test-object-map.cpp
test_object_mapincludedir	= $(includedir)/Atlas/src/TestSuite
test_object_map_LDADD		= ../Message/.libs/libAtlasMessage.so

test_object_list_SOURCES	= test-object-list.cpp
test_object_listincludedir	= $(includedir)/Atlas/src/TestSuite
test_object_list_LDADD		= ../Message/.libs/libAtlasMessage.so

test_negotiate_SOURCES		= test-negotiate.cc
test_negotiateincludedir	= $(includedir)/Atlas/src/TestSuite
test_negotiate_LDADD		= ../.libs/libAtlas.so ../Message/.libs/libAtlasMessage.a ../Codecs/.libs/libAtlasCodecs.so ../Filters/.libs/libAtlasFilters.so -lz -lbz2

test_packed_SOURCES		= test-packed.cc
test_packed_LDADD		= ../.libs/libAtlas.so ../Message/.libs/libAtlasMessage.a ../Codecs/.libs/libAtlasCodecs.so ../Filters/.libs/libAtlasFilters.a ../Funky/.libs/libAtlasFunky.so

test_xml_SOURCES	    = test-xml.cc
test_xml_LDADD		    = ../.libs/libAtlas.so ../Message/.libs/libAtlasMessage.a ../Codecs/.libs/libAtlasCodecs.so ../Filters/.libs/libAtlasFilters.a ../Funky/.libs/libAtlasFunky.so

TESTS = test-object-map test-object-list test-negotiate

