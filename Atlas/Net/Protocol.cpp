/*
        AtlasProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cassert>

#include "Protocol.h"
#include "ProtocolDecoder.h"
#include "ProtocolEncoder.h"

AProtocol::~AProtocol() {
    delete encoder;
    delete decoder;
}

AProtocolEncoder*	AProtocol::getEncoder() {
    assert( encoder != 0 );
    return encoder;
}

AProtocolDecoder*	AProtocol::getDecoder() {
    assert( decoder != 0 );
    return decoder;
}

int AProtocol::atlasERRTOK = -1;  // error token
int AProtocol::atlasMSGBEG = 1;   // message begins
int AProtocol::atlasATRBEG = 2;   // attribute begins
int AProtocol::atlasATRVAL = 3;   // attribute value
int AProtocol::atlasATREND = 4;   // attribute end
int AProtocol::atlasMSGEND = 5;   // message ends

