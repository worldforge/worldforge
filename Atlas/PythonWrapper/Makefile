all: ccAtlasObjectmodule.so ccAtlasNetcmodule.so

wrapperNet.cpp: wrapperNet.i Socket.i TCPSocket.i Client.i Codec.i \
		Protocol.i XMLProtocol.i
	swig -python -shadow -c++ -o wrapperNet.cpp wrapperNet.i

ccAtlasNetcmodule.so: wrapperNet.cpp wrapperObject.h
	g++ -g -Wall --shared -o ccAtlasNetcmodule.so wrapperNet.cpp -I/usr/include/python1.5 -lAtlasObject -lAtlasNet

ccAtlasObjectmodule.so: wrapperObject.cpp
	g++ -g -Wall --shared -o ccAtlasObjectmodule.so wrapperObject.cpp -I/usr/include/python1.5 -lAtlasObject -lAtlasNet
