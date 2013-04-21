
AM_CPPFLAGS = -I$(top_srcdir)

bin_PROGRAMS = atlas_convert

atlas_convert_SOURCES = atlas_convert.cpp

atlas_convert_LDADD = $(top_builddir)/Atlas/Message/libAtlasMessage-0.7.la \
                      $(top_builddir)/Atlas/Codecs/libAtlasCodecs-0.7.la \
                      $(top_builddir)/Atlas/libAtlas-0.7.la
