/*
        AtlasProtocolEncoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasProtocolEncoder.h"

AProtocolEncoder::AProtocolEncoder()
{
	size = 1024;
	buffer = (char*)malloc(1024);
}

void AProtocolEncoder::printf(char* fmt, ...)
{
	int	newsiz;
	char	str[2048];
	va_list	va;
	
	va_start(va,fmt);
	vsprintf(str, fmt, va);
	va_end(va);

	newsiz = strlen(buffer)+strlen(str)+1;
	if (newsiz > size) {
		size = newsiz+256;
		char* tmp = (char*)malloc(size);
		strcpy(tmp,buffer);
		free(buffer);
		buffer = tmp;
	}
	strcat(buffer,str);
	bufsiz += strlen(str);
}
		
void AProtocolEncoder::append(char* data, int len)
{
	if (bufsiz+len+1 > size) {
		size = bufsiz+len+256;
		char* tmp = (char*)malloc(size);
		memcpy(tmp,buffer,bufsiz);
		free(buffer);
		buffer = tmp;
	}
	memcpy(buffer+bufsiz,data,len);
	bufsiz += len;
}
		
char* AProtocolEncoder::encodeMessage(AObject *msg)
{
	return NULL;
}

int AProtocolEncoder::encodedLength()
{
	return bufsiz;
}

