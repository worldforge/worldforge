/*
        AtlasProtocolDecoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocolDecoder_h_
#define __AtlasProtocolDecoder_h_

#include "AtlasObject.h"

class AProtocolDecoder
{
protected:
	char*	buffer;
	char	name[80];
	int	type;
	char*	sval;
	long	ival;
	double	fval;


public:
	AProtocolDecoder();

	char*	getName();
	int	getType();
	char*	getString();
	long	getInt();
	double	getFloat();


	virtual void	newStream();
	virtual void	feedStream(char* data, int len);
	virtual int	hasTokens();
	virtual int	getToken();


};

#endif


