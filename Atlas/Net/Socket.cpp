/*
        AtlasSocket.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cstdio>
#include <cstdlib>
#include <memory.h>

#include "Socket.h"
#include "../Object/Debug.h"
#include <sys/types.h>

int ASocket::connect(const string& addr, int port)		
{
	DebugMsg1(0, "ASocket :: connect() not overidden in subclass", "");
	return -1;
}

int ASocket::listen(const string& addr, int port, int blog)
{
	DebugMsg1(0, "ASocket :: listen() not overidden in subclass", "");
	return -1;
}

int ASocket::send(const string& data)
{
	DebugMsg1(0, "ASocket :: send() not overidden in subclass", "");
	return -1;
}

int ASocket::sendTo(const string& data, const sockaddr_in& addr)
{
	DebugMsg1(0, "ASocket :: sendTo() not overidden in subclass", "");
	return -1;
}

int ASocket::recv(string& buf)
{
	DebugMsg1(0, "ASocket :: recv() not overidden in subclass", "");
	return -1;
}

int ASocket::recvFrom(string& buf, const sockaddr_in& addr)
{
	DebugMsg1(0, "ASocket :: recvFrom() not overidden in subclass", "");
	return -1;
}

void ASocket::close()
{
	sock = (SOCKET)-1;
}

SOCKET ASocket::getSock()				{return sock;}


