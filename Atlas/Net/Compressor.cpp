/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Compressor.h"

namespace Atlas
{

string  Compressor::encode( const string& data)
{
    return data;
}

string  Compressor::decode( const string& data)
{
    return data;
}

void		Compressor::resetEncoder() {}
void		Compressor::resetDecoder() {}

} // namespace Atlas
