
lib_LTLIBRARIES = libAtlasObjects.la

libAtlasObjects_la_LDFLAGS = $(INTERFACE_VERSION)

libAtlasObjectsincludedir = $(includedir)/Atlas/Objects

REAL_GENERATED_CPP = Root.cpp Decoder.cpp \
	RootEntity.cpp RootEntityChildren.cpp \
        RootOperation.cpp RootOperationChildren1.cpp \
	RootOperationChildren2.cpp RootOperationChildren3.cpp \
	RootOperationChildren4.cpp RootOperationChildren5.cpp \
	Anonymous.cpp

GENERATED_CPP = flag.cpp $(REAL_GENERATED_CPP)

GENERATED_H = Root.h Decoder.h \
	RootEntity.h Entity.h \
        RootOperation.h Operation.h \
	Anonymous.h

libAtlasObjects_la_SOURCES = \
	gen_cpp.py BaseObject.cpp loadDefaults.cpp objectFactory.cpp \
	$(GENERATED_CPP)

BUILT_SOURCES = $(GENERATED_CPP) $(GENERATED_H)


PY_SOURCES = gen_cpp.py common.py AttributeInfo.py GenerateObjectFactory.py \
	GenerateDecoder.py

$(REAL_GENERATED_CPP) $(GENERATED_H):
	if [ ! -e $@ ]; \
	then \
		python gen_cpp.py ; \
	fi

flag.cpp: $(PY_SOURCES)
	python gen_cpp.py && touch flag.cpp

libAtlasObjectsinclude_HEADERS = \
	BaseObject.h SmartPtr.h Encoder.h objectFactory.h loadDefaults.h \
	$(GENERATED_H)

EXTRA_DIST = $(PY_SOURCES) README DESIGN TODO
