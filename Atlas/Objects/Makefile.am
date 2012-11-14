
INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES = libAtlasObjects-0.6.la

libAtlasObjects_0_6_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasObjects_0_6_la_LIBADD = $(top_builddir)/Atlas/libAtlas-0.6.la \
    $(top_builddir)/Atlas/Message/libAtlasMessage-0.6.la \
    $(top_builddir)/Atlas/Codecs/libAtlasCodecs-0.6.la

REAL_GENERATED_CPP = Root.cpp Decoder.cpp Dispatcher.cpp objectFactory.cpp \
	RootEntity.cpp RootEntityChildren.cpp \
	RootOperation.cpp RootOperationChildren1.cpp \
	RootOperationChildren2.cpp RootOperationChildren3.cpp \
	RootOperationChildren4.cpp RootOperationChildren5.cpp \
	Anonymous.cpp Generic.cpp

GENERATED_CPP = flag.cpp $(REAL_GENERATED_CPP)

GENERATED_H = Root.h Decoder.h Dispatcher.h ObjectsFwd.h \
	RootEntity.h Entity.h \
	RootOperation.h Operation.h \
	Anonymous.h Generic.h

libAtlasObjects_0_6_la_SOURCES = \
	gen_cpp.py BaseObject.cpp SmartPtr.cpp Encoder.cpp \
	loadDefaults.cpp \
	$(GENERATED_CPP)

BUILT_SOURCES = $(GENERATED_CPP) $(GENERATED_H)

PY_SOURCES = gen_cpp.py common.py AttributeInfo.py GenerateObjectFactory.py \
	GenerateDecoder.py GenerateDispatcher.py GenerateForward.py

$(REAL_GENERATED_CPP) $(GENERATED_H):
	if [ ! -e $@ ]; \
	then \
		cd $(srcdir) && python gen_cpp.py ; \
	fi

flag.cpp: $(PY_SOURCES)
	cd $(srcdir) && python gen_cpp.py && (echo /\* ; date --rfc-3339=seconds ; echo \*/) > flag.cpp

libAtlasObjectsincludedir = $(includedir)/Atlas-C++-0.6/Atlas/Objects

libAtlasObjectsinclude_HEADERS = \
	BaseObject.h SmartPtr.h Encoder.h objectFactory.h loadDefaults.h \
	$(GENERATED_H)

EXTRA_DIST = $(PY_SOURCES) README DESIGN TODO
