/*
        AtlasPackedAsciiDecoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocolDecoder__h_
#define __AtlasProtocolDecoder__h_

#include "ProtocolDecoder.h"

#include <string.h>
#include <stdlib.h>
#include <memory.h>

/*
    The Packed ASCII protocol produces messages that look like:

{pos=$alt=49000(coord=#X=4865.5#Y=1376.4)}

	The decoding method is a state machine.

	States:
	1. waiting for start of message
		if "{" and "=" found
			pull out message name
			goto state 2

		2. waiting for the "["
        if "[" found
            extract name
            store token atlasBEGMSG
            nestd++
            goto state 3

    3. waiting for "a" or "]"
        if "a" found
            goto state 5
        if "]" found
            nestd--
            goto state 4
        if anything else found
            goto state ERROR

    4. waiting for ">"
        if ">" found
            if nestd = 0 
                store token atlasENDMSG
                goto state 1
            else
                store token atlasENDATR
                goto state 3

    5. waiting for "="
        if "=" found
            extract name and type
            store token atlasBEGATR
            if type = list goto state 7
            if type != list goto state 6

    6. waiting for ">"
        if ">" found
            extract value
            store token atlasATRVAL
            goto state 7

    7. waiting for nothing
        store token atlasENDATR
        goto state 3

    8. waiting for "["
        if "[" found
            nestd++
            goto state 3
        if anything else found
            goto state ERROR

*/


class APackedAsciiDecoder: public AProtocolDecoder
{

private:
    int    state;
    int    nestd;
    int    token;


public:

    APackedAsciiDecoder()
    {
        state = 1;
        nestd = 0;
        token = 0;
    }

    ~APackedAsciiDecoder()
    {
    }

    void newStream();
    void feedStream(const string& data);
    int getToken();
    int hasTokens();

};

#endif



