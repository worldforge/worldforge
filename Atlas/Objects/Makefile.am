lib_LTLIBRARIES = libAtlasObjects.la

libAtlasObjectsincludedir = $(includedir)/Atlas/Objects

GENERATED_CPP = flag.cpp Root.cpp Decoder.cpp \
	RootEntity.cpp RootEntityChildren.cpp \
        RootOperation.cpp RootOperationChildren1.cpp \
	RootOperationChildren2.cpp RootOperationChildren3.cpp \
	RootOperationChildren4.cpp RootOperationChildren5.cpp

GENERATED_H = Root.h Decoder.h \
	loadDefaults.h objectFactory.h \
	RootEntity.h Entity.h \
        RootOperation.h Operation.h

libAtlasObjects_la_SOURCES = \
	gen_cpp.py BaseObject.cpp loadDefaults.cpp objectFactory.cpp \
	$(GENERATED_CPP)

BUILT_SOURCES = $(GENERATED_CPP) $(GENERATED_H)


PY_SOURCES = gen_cpp.py common.py AttributeInfo.py GenerateObjectFactory.py \
	GenerateDecoder.py

flag.cpp: $(PY_SOURCES)
	gen_cpp.py && touch flag.cpp

libAtlasObjectsinclude_HEADERS = \
	BaseObject.h SmartPtr.h Encoder.h $(GENERATED_H)

EXTRA_DIST = $(PY_SOURCES) README DESIGN TODO
