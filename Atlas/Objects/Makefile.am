lib_LTLIBRARIES = libAtlasObjects.la

libAtlasObjectsincludedir = $(includedir)/Atlas/Objects

libAtlasObjects_la_SOURCES = \
	gen_cpp.py flag.cpp BaseObject.cpp Root.cpp Decoder.cpp \
	loadDefaults.cpp objectFactory.cpp \
	RootEntity.cpp RootEntityChildren.cpp \
        RootOperation.cpp RootOperationChildren1.cpp \
	RootOperationChildren2.cpp RootOperationChildren3.cpp \
	RootOperationChildren4.cpp RootOperationChildren5.cpp

BUILT_SOURCES = Root.cpp Root.h

flag.cpp: gen_cpp.py common.py AttributeInfo.py GenerateObjectFactory.py \
	GenerateDecoder.py
	gen_cpp.py && touch flag.cpp

libAtlasObjectsinclude_HEADERS = \
	BaseObject.h SmartPtr.h Root.h Encoder.h Decoder.h \
	loadDefaults.h objectFactory.h \
	RootEntity.h Entity.h \
        RootOperation.h Operation.h

EXTRA_DIST = gen_cpp.py common.py attribute_info.py README DESIGN TODO
