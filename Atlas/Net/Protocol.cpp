/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cassert>

#include "Protocol.h"
#include "Decoder.h"
#include "Encoder.h"

namespace Atlas
{

Protocol::~Protocol() {
    delete encoder;
    delete decoder;
}

Encoder*	Protocol::getEncoder() {
    assert( encoder != 0 );
    return encoder;
}

Decoder*	Protocol::getDecoder() {
    assert( decoder != 0 );
    return decoder;
}

int Protocol::atlasERRTOK = -1;  // error token
int Protocol::atlasMSGBEG = 1;   // message begins
int Protocol::atlasATRBEG = 2;   // attribute begins
int Protocol::atlasATRVAL = 3;   // attribute value
int Protocol::atlasATREND = 4;   // attribute end
int Protocol::atlasMSGEND = 5;   // message ends

} // namespace Atlas
