/*
        AtlasSocket.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasSocket.h"

ASocket::ASocket()
{
	sock = -1;
}

ASocket::ASocket(int asock)
{
	sock = asock;
}

ASocket::~ASocket()
{
}
ASocket* ASocket::accept()				{return NULL;}


int ASocket::connect(string& addr, int port)		{return -1;}

int ASocket::listen(string& addr, int port, int blog)	{return -1;}

int ASocket::send(string& data)				{return -1;}

int ASocket::sendTo(string& data, sockaddr_in& addr)	{return -1;}

int ASocket::recv(string& buf)				{return -1;}

int ASocket::recvFrom(string& buf, sockaddr_in& addr)	{return -1;}

void ASocket::close()					
{
	sock = -1;
}

int ASocket::getSock()					{return sock;}





