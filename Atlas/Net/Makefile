
# Standard defines:
CC     	=	g++
AR     	=	ar
RANLIB  =	ranlib
libDirs	=	-L$(OUTDIR)
incDirs	=	-I. -I/usr/include/python1.5
LIBS	=	-lstdc++ -lpython1.5 -lAtlas -ldl
#C_FLAGS	=	-g -D_ADebug_
C_FLAGS	=	-O3
EXE_EXT =
MK_DEP  =	$(CC) $(C_FLAGS) $(incDirs) -MM

OUTDIR	=	linux
#OUTDIR	=	win32

# Targets follow ---------------------------------

TARGETS = \
	$(OUTDIR)/libAtlas.a \
	$(OUTDIR)/CodecBench$(EXE_EXT) \
	$(OUTDIR)/SocketTest$(EXE_EXT) \
	$(OUTDIR)/XMLTest$(EXE_EXT) \
	$(OUTDIR)/PackedAsciiTest$(EXE_EXT) \

LIBOBJS = \
	AtlasCodec.o \
	AtlasPackedAsciiDecoder.o \
	AtlasPackedAsciiEncoder.o\
	AtlasPackedAsciiProtocol.o\
	AtlasXMLDecoder.o \
	AtlasXMLEncoder.o\
	AtlasXMLProtocol.o\
	AtlasProtocol.o\
	AtlasProtocolDecoder.o\
	AtlasProtocolEncoder.o\
	AtlasTCPSocket.o\
	AtlasSocket.o\
	AtlasServer.o\
	AtlasClient.o\
	AtlasObject.o\
	AtlasDebug.o

all:		$(TARGETS)

cleanobjs:
		rm -f $(OUTDIR)/*.o $(OUTDIR)/*.a

cleanbin:
		rm -f $(TARGETS)

clean:	 	cleanobjs

cleanall:	cleanobjs cleanbin

makedeps:
	rm Makefile.dep
	$(MK_DEP) AtlasObject.cpp		>>Makefile.dep
	$(MK_DEP) AtlasCodec.cpp		>>Makefile.dep
	$(MK_DEP) AtlasClient.cpp		>>Makefile.dep
	$(MK_DEP) AtlasCodec.cpp		>>Makefile.dep
	$(MK_DEP) AtlasDebug.cpp		>>Makefile.dep
	$(MK_DEP) AtlasObject.cpp		>>Makefile.dep
	$(MK_DEP) AtlasPackedAsciiDecoder.cpp	>>Makefile.dep
	$(MK_DEP) AtlasPackedAsciiEncoder.cpp	>>Makefile.dep
	$(MK_DEP) AtlasPackedAsciiProtocol.cpp	>>Makefile.dep
	$(MK_DEP) AtlasProtocol.cpp		>>Makefile.dep
	$(MK_DEP) AtlasProtocolDecoder.cpp	>>Makefile.dep
	$(MK_DEP) AtlasProtocolEncoder.cpp	>>Makefile.dep
	$(MK_DEP) AtlasServer.cpp		>>Makefile.dep
	$(MK_DEP) AtlasSocket.cpp		>>Makefile.dep
	$(MK_DEP) AtlasTCPSocket.cpp		>>Makefile.dep
	$(MK_DEP) AtlasXMLDecoder.cpp		>>Makefile.dep
	$(MK_DEP) AtlasXMLEncoder.cpp		>>Makefile.dep
	$(MK_DEP) AtlasXMLProtocol.cpp		>>Makefile.dep
	$(MK_DEP) ChatClient.cpp		>>Makefile.dep
	$(MK_DEP) ChatServer.cpp		>>Makefile.dep
	$(MK_DEP) CodecBench.cpp		>>Makefile.dep
	$(MK_DEP) PackedAsciiTest.cpp		>>Makefile.dep
	$(MK_DEP) SocketTest.cpp		>>Makefile.dep
	$(MK_DEP) XMLTest.cpp			>>Makefile.dep

# custom make rules

.cpp.o:
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

# Target rules follow -----------------------------

$(OUTDIR)/libAtlas.a: $(LIBOBJS)
	$(AR) -r $(OUTDIR)/libAtlas.a $(LIBOBJS) 

$(OUTDIR)/PackedAsciiTest$(EXE_EXT): PackedAsciiTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/PackedAsciiTest$(EXE_EXT) PackedAsciiTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/XMLTest$(EXE_EXT): XMLTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/XMLTest$(EXE_EXT) XMLTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/SocketTest$(EXE_EXT): SocketTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/SocketTest$(EXE_EXT) SocketTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/CodecBench$(EXE_EXT): CodecBench.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/CodecBench$(EXE_EXT) CodecBench.o $(incDirs) $(libDirs) $(LIBS)

# Dependency rules follow -----------------------------

include Makefile.dep
