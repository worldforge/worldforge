
bin_PROGRAMS = atlas_convert

INCLUDES = -I$(top_srcdir)/src

atlas_convert_SOURCES = atlas_convert.cpp

atlas_convert_LDFLAGS = \
	-L$(top_builddir)/src -L$(top_builddir)/src/Codecs -L$(top_builddir)/src/Message \
	-lAtlas-0.6 -lAtlasCodecs-0.6 -lAtlasMessage-0.6
