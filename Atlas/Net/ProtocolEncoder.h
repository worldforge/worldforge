/*
        AtlasProtocolEncoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocolEncoder_h_
#define __AtlasProtocolEncoder_h_

#include "Object.h"

class AProtocolEncoder
{

protected:
	string          buffer;
	string hexEncodeString(const string& input, char prefix,
			       const string& specialchars);
	
public:
	AProtocolEncoder();
	virtual ~AProtocolEncoder() {};
	void printf(char* fmt, ...);
	void append(string& data);
	virtual string encodeMessage(const AObject& msg);
	virtual int encodedLength();	
};

#endif



