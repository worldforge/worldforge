all: ccAtlasObjectmodule.so ccAtlasNetcmodule.so

wrapperNet.cpp: wrapperNet.i Socket.i TCPSocket.i Codec.i \
		Protocol.i XMLProtocol.i PackedProtocol.i wrapperClient.h
	swig -python -shadow -c++ -o wrapperNet.cpp wrapperNet.i

wrapperClient.o: wrapperClient.cpp wrapperClient.h
	g++ -g -Wall -c -o wrapperClient.o wrapperClient.cpp -I/usr/include/python1.5

ccAtlasNetcmodule.so: wrapperNet.cpp wrapperObject.h wrapperClient.o
	g++ -g -Wall --shared -o ccAtlasNetcmodule.so wrapperClient.o wrapperNet.cpp -I/usr/include/python1.5 -lAtlasObject -lAtlasNet

ccAtlasObjectmodule.so: wrapperObject.cpp
	g++ -g -Wall --shared -o ccAtlasObjectmodule.so wrapperObject.cpp -I/usr/include/python1.5 -lAtlasObject -lAtlasNet
