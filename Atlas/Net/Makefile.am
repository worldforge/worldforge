
lib_LTLIBRARIES = libAtlasNet.la

libAtlasNet_la_SOURCES = \
	Client.cpp Codec.cpp Compressor.cpp \
	PackedAsciiDecoder.cpp PackedAsciiEncoder.cpp \
	PackedAsciiProtocol.cpp Protocol.cpp Decoder.cpp \
	Encoder.cpp Server.cpp Socket.cpp TCPSocket.cpp \
	XMLDecoder.cpp XMLEncoder.cpp XMLProtocol.cpp

libAtlasNetincludedir = $(includedir)/Atlas/Net

libAtlasNetinclude_HEADERS = \
	Client.h Codec.h Compressor.h \
	Net.h PackedAsciiDecoder.h PackedAsciiEncoder.h \
	PackedAsciiProtocol.h Protocol.h Decoder.h \
	Encoder.h Server.h Socket.h TCPSocket.h \
	XMLDecoder.h XMLEncoder.h XMLProtocol.h

CXXFLAGS = -Wall -g -D_ADebug_
