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
	setupsock(-1,256);
}

ASocket::ASocket(int recvbufsize)
{
	setupsock(-1, rbufsize);
}

ASocket::ASocket(int asock, int recvbufsize)
{
	setupsock(asock, rbufsize);
}

ASocket::~ASocket()
{
	free(rbuf);
}

void ASocket::setupsock(int asock, int bufsz)
{
	sock = asock;
	rbuf = (char*)malloc(bufsz);
}

int ASocket::connect(char* addr, int port)		{printf("base connect\n"); return -1;}
int ASocket::listen(char* addr, int port, int backlog)	{return -1;}
ASocket* ASocket::accept()				{return NULL;}
int ASocket::send(char* data)				{return -1;}
int ASocket::send(char* data, size_t len)		{return -1;}
int ASocket::sendTo(char* data, sockaddr_in* addr)	{return -1;}
int ASocket::sendTo(char* data, size_t len, sockaddr_in* addr)	{return -1;}
char* ASocket::recv()					{return NULL;}
int ASocket::recv(char* buf, int len)			{return -1;}
char* ASocket::recvFrom(sockaddr_in* addr)		{return NULL;}
void ASocket::close()					
{
	sock = -1;
}
int ASocket::getSock()					{return sock;}





