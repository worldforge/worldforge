/*
        AtlasEncoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasEncoder_h_
#define __AtlasEncoder_h_

#include "../Object/Object.h"

namespace Atlas
{

class Encoder
{

protected:
	string          buffer;
	string hexEncodeString(const string& input, char prefix,
			       const string& specialchars);
	
public:
	Encoder();
	virtual ~Encoder() {};
	void printf(char* fmt, ...);
	void append(string& data);
	virtual string encodeMessage(const Object& msg);
	virtual int encodedLength();	
};

} // namespace Atlas

#endif



