
AM_CPPFLAGS = -I$(top_srcdir)

LDADD  = -L$(top_builddir)/Atlas/ -L$(top_builddir)/Atlas/Codecs/ \
         -L$(top_builddir)/Atlas/Filters/ \
         -L$(top_builddir)/Atlas/Message/ -L$(top_builddir)/Atlas/Net/ \
         -L$(top_builddir)/Atlas/Objects/

LIBS = -lAtlasObjects-0.6 -lAtlasNet-0.6 -lAtlas-0.6 -lAtlasCodecs-0.6 \
       -lAtlasFilters-0.6 -lAtlasMessage-0.6 -lAtlas-0.6 $(OTHER_LIBS)

check_PROGRAMS = custom_ops objects1 objects2 encoder1 decoder1 objects_fwd attributes flags

TESTS = $(check_PROGRAMS)

custom_ops_SOURCES = custom_ops.cpp

objects1_SOURCES = objects1.cpp

objects2_SOURCES = objects2.cpp DebugBridge.h

encoder1_SOURCES = encoder1.cpp

decoder1_SOURCES = decoder1.cpp

objects_fwd_SOURCES = objects_fwd.cpp

attributes_SOURCES = attributes.cpp

flags_SOURCES = flags.cpp
