
# Standard defines:
CC     	=	g++
AR     	=	ar
RANLIB  =	ranlib
libDirs	=	-L$(OUTDIR)
incDirs	=	-I. -I/usr/include/python1.5
C_FLAGS	=	-g -D_ADebug_
# C_FLAGS	=	-O3
MK_DEP  =	$(CC) $(C_FLAGS) $(incDirs) -MM

OUTDIR	=	linux
EXE_EXT =
LIBS	=	-lstdc++ -lpython1.5 -lAtlas -ldl

#OUTDIR  =       win32
#EXE_EXT =	.exe
#LIBS	=	-lAtlas -lpython15 -lwsock32 -lstdc++

# Targets follow ---------------------------------

TARGETS = \
	$(OUTDIR)/libAtlas.a \
	$(OUTDIR)/CodecBench$(EXE_EXT) \
	$(OUTDIR)/SocketTest$(EXE_EXT) \
	$(OUTDIR)/XMLTest$(EXE_EXT) \
	$(OUTDIR)/PackedAsciiTest$(EXE_EXT) \
	$(OUTDIR)/EchoTest$(EXE_EXT) \
	$(OUTDIR)/CopyTest$(EXE_EXT) \

LIBOBJS = \
	$(OUTDIR)/AtlasCodec.o \
	$(OUTDIR)/AtlasPackedAsciiDecoder.o \
	$(OUTDIR)/AtlasPackedAsciiEncoder.o \
	$(OUTDIR)/AtlasPackedAsciiProtocol.o \
	$(OUTDIR)/AtlasXMLDecoder.o \
	$(OUTDIR)/AtlasXMLEncoder.o \
	$(OUTDIR)/AtlasXMLProtocol.o \
	$(OUTDIR)/AtlasProtocol.o \
	$(OUTDIR)/AtlasProtocolDecoder.o \
	$(OUTDIR)/AtlasProtocolEncoder.o \
	$(OUTDIR)/AtlasTCPSocket.o \
	$(OUTDIR)/AtlasSocket.o \
	$(OUTDIR)/AtlasServer.o \
	$(OUTDIR)/AtlasClient.o \
	$(OUTDIR)/AtlasObject.o \
	$(OUTDIR)/AtlasPython.o \
	$(OUTDIR)/AtlasDebug.o \
	$(OUTDIR)/AtlasCompressor.o

all:		$(TARGETS)

cleanobjs:
		rm -f $(OUTDIR)/*.o $(OUTDIR)/*.a

cleanbin:
		rm -f $(TARGETS)

clean:	 	cleanobjs

cleanall:	cleanobjs cleanbin

# custom make rules

.cpp.o:
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

# Target rules follow -----------------------------

$(OUTDIR)/libAtlas.a: $(LIBOBJS)
	$(AR) -r $(OUTDIR)/libAtlas.a $(LIBOBJS) 

$(OUTDIR)/PackedAsciiTest$(EXE_EXT): $(OUTDIR)/PackedAsciiTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/PackedAsciiTest$(EXE_EXT) $(OUTDIR)/PackedAsciiTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/XMLTest$(EXE_EXT): $(OUTDIR)/XMLTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/XMLTest$(EXE_EXT) $(OUTDIR)/XMLTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/SocketTest$(EXE_EXT): $(OUTDIR)/SocketTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/SocketTest$(EXE_EXT) $(OUTDIR)/SocketTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/CodecBench$(EXE_EXT): $(OUTDIR)/CodecBench.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/CodecBench$(EXE_EXT) $(OUTDIR)/CodecBench.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/EchoTest$(EXE_EXT): $(OUTDIR)/EchoTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/EchoTest$(EXE_EXT) $(OUTDIR)/EchoTest.o $(incDirs) $(libDirs) $(LIBS)

$(OUTDIR)/CopyTest$(EXE_EXT): $(OUTDIR)/CopyTest.o $(OUTDIR)/libAtlas.a
	$(CC) -o $(OUTDIR)/CopyTest$(EXE_EXT) $(OUTDIR)/CopyTest.o $(incDirs) $(libDirs) $(LIBS)

# Dependency rules follow -----------------------------

$(OUTDIR)/AtlasObject.o: AtlasObject.cpp AtlasObject.h AtlasDebug.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasPython.o: AtlasPython.cpp AtlasPython.h AtlasDebug.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasCodec.o: AtlasCodec.cpp AtlasCodec.h AtlasDebug.h AtlasObject.h \
	AtlasProtocol.h AtlasProtocolEncoder.h AtlasProtocolDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasClient.o: AtlasClient.cpp AtlasClient.h AtlasCodec.h AtlasDebug.h \
	AtlasObject.h AtlasProtocol.h AtlasProtocolEncoder.h \
	AtlasProtocolDecoder.h AtlasSocket.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasDebug.o: AtlasDebug.cpp AtlasDebug.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasPackedAsciiDecoder.o: AtlasPackedAsciiDecoder.cpp AtlasObject.h \
	AtlasDebug.h AtlasProtocol.h AtlasProtocolEncoder.h \
	AtlasProtocolDecoder.h AtlasPackedAsciiDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasPackedAsciiEncoder.o: AtlasPackedAsciiEncoder.cpp \
	AtlasPackedAsciiEncoder.h AtlasObject.h AtlasDebug.h \
	AtlasProtocolEncoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasPackedAsciiProtocol.o: AtlasPackedAsciiProtocol.cpp \
	AtlasProtocol.h AtlasProtocolEncoder.h AtlasObject.h AtlasDebug.h \
	AtlasProtocolDecoder.h AtlasPackedAsciiProtocol.h \
	AtlasPackedAsciiEncoder.h AtlasPackedAsciiDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasProtocol.o: AtlasProtocol.cpp AtlasProtocol.h \
	AtlasProtocolEncoder.h AtlasObject.h AtlasDebug.h \
	AtlasProtocolDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasProtocolDecoder.o: AtlasProtocolDecoder.cpp \
	AtlasProtocolDecoder.h AtlasObject.h AtlasDebug.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasProtocolEncoder.o: AtlasProtocolEncoder.cpp \
	AtlasProtocolEncoder.h AtlasObject.h AtlasDebug.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasServer.o: AtlasServer.cpp AtlasServer.h AtlasSocket.h \
	AtlasClient.h AtlasCodec.h AtlasDebug.h AtlasObject.h AtlasProtocol.h \
	AtlasProtocolEncoder.h AtlasProtocolDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasSocket.o: AtlasSocket.cpp AtlasSocket.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasTCPSocket.o: AtlasTCPSocket.cpp AtlasTCPSocket.h AtlasSocket.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasXMLDecoder.o: AtlasXMLDecoder.cpp AtlasObject.h AtlasDebug.h \
	AtlasProtocol.h AtlasProtocolEncoder.h AtlasProtocolDecoder.h \
	AtlasXMLDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasXMLEncoder.o: AtlasXMLEncoder.cpp AtlasXMLEncoder.h AtlasObject.h \
	AtlasDebug.h AtlasProtocolEncoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasXMLProtocol.o: AtlasXMLProtocol.cpp AtlasProtocol.h \
	AtlasProtocolEncoder.h AtlasObject.h AtlasDebug.h \
	AtlasProtocolDecoder.h AtlasXMLProtocol.h AtlasXMLEncoder.h \
	AtlasXMLDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/AtlasCompressor.o: AtlasCompressor.cpp AtlasCompressor.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/ChatClient.o: ChatClient.cpp ChatClient.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/ChatServer.o: ChatServer.cpp SocketTest.h AtlasSocket.h \
	AtlasTCPSocket.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/CodecBench.o: CodecBench.cpp AtlasCodec.h AtlasDebug.h AtlasObject.h \
	AtlasProtocol.h AtlasProtocolEncoder.h AtlasProtocolDecoder.h \
	AtlasXMLProtocol.h AtlasXMLEncoder.h AtlasXMLDecoder.h \
	AtlasPackedAsciiProtocol.h AtlasPackedAsciiEncoder.h \
	AtlasPackedAsciiDecoder.h CodecBench.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/PackedAsciiTest.o: PackedAsciiTest.cpp AtlasCodec.h AtlasDebug.h \
	AtlasObject.h AtlasProtocol.h AtlasProtocolEncoder.h \
	AtlasProtocolDecoder.h AtlasPackedAsciiProtocol.h \
	AtlasPackedAsciiEncoder.h AtlasPackedAsciiDecoder.h PackedAsciiTest.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/SocketTest.o: SocketTest.cpp SocketTest.h AtlasSocket.h \
	AtlasTCPSocket.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/XMLTest.o: XMLTest.cpp XMLTest.h AtlasObject.h AtlasDebug.h \
	AtlasCodec.h AtlasProtocol.h AtlasProtocolEncoder.h \
	AtlasProtocolDecoder.h AtlasXMLProtocol.h AtlasXMLEncoder.h \
	AtlasXMLDecoder.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/EchoTest.o: EchoTest.cpp EchoTest.h AtlasClient.h AtlasCodec.h \
	AtlasDebug.h AtlasObject.h AtlasProtocol.h AtlasProtocolEncoder.h \
	AtlasProtocolDecoder.h AtlasSocket.h AtlasCompressor.h \
	AtlasPackedAsciiProtocol.h AtlasPackedAsciiEncoder.h \
	AtlasPackedAsciiDecoder.h AtlasTCPSocket.h
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)

$(OUTDIR)/CopyTest.o: CopyTest.cpp CopyTest.h AtlasDebug.h AtlasObject.h 
	$(CC) $(C_FLAGS) -c -o $@ $< $(incDirs)



