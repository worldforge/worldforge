
bin_PROGRAMS = atlas_convert

atlas_convert_SOURCES = atlas_convert.cpp

atlas_convert_LDFLAGS = \
	-L$(top_builddir)/Atlas -L$(top_builddir)/Atlas/Codecs \
	-L$(top_builddir)/Atlas/Message \
	-lAtlas-0.6 -lAtlasCodecs-0.6 -lAtlasMessage-0.6
