/*
        AtlasProtocolDecoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocolDecoder_h_
#define __AtlasProtocolDecoder_h_

#include "../Object/Object.h"

namespace Atlas
{

class ProtocolDecoder
{
protected:
	string	buffer;
	string	name;
	int	type;
	string	sval;
	long	ival;
	double	fval;

	string hexDecodeString(const string& input, char prefix);


public:
	ProtocolDecoder();
	virtual ~ProtocolDecoder() {}

	string	getName();
	int	getType();
	string	getString();
	long	getInt();
	double	getFloat();


	virtual void	newStream();
	virtual void	feedStream(const string& data);
	virtual int	hasTokens();
	virtual int	getToken();


};

} // namespace Atlas


#endif


