/*
        AtlasSocket.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Socket.h"

ASocket::ASocket()
{
	sock = (SOCKET)-1;
}

ASocket::ASocket(SOCKET asock)
{
	sock = asock;
}


ASocket* ASocket::accept()				{return NULL;}


int ASocket::connect(string& addr, int port)		
{
	DebugMsg1(0, "ASocket::connect BAD VIRTUAL CALL !!!", "");
	return -1;
}

int ASocket::listen(string& addr, int port, int blog)
{
	DebugMsg1(0, "ASocket::listen BAD VIRTUAL CALL !!!", "");
	return -1;
}

int ASocket::send(string& data)
{
	DebugMsg1(0, "ASocket::send BAD VIRTUAL CALL !!!", "");
	return -1;
}

int ASocket::sendTo(string& data, sockaddr_in& addr)
{
	DebugMsg1(0, "ASocket::sendTo BAD VIRTUAL CALL !!!", "");
	return -1;
}

int ASocket::recv(string& buf)
{
	DebugMsg1(0, "ASocket::recv BAD VIRTUAL CALL !!!", "");
	return -1;
}

int ASocket::recvFrom(string& buf, sockaddr_in& addr)
{
	DebugMsg1(0, "ASocket::recvFrom BAD VIRTUAL CALL !!!", "");
	return -1;
}

void ASocket::close()
{
	sock = (SOCKET)-1;
}

SOCKET ASocket::getSock()				{return sock;}


