/*
        AtlasProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasProtocol.h"

string	AProtocol::prefix("");

int AProtocol::atlasERRTOK = -1;  // error token
int AProtocol::atlasMSGBEG = 1;   // message begins
int AProtocol::atlasATRBEG = 2;   // attribute begins
int AProtocol::atlasATRVAL = 3;   // attribute value
int AProtocol::atlasATREND = 4;   // attribute end
int AProtocol::atlasMSGEND = 5;   // message ends

int AProtocol::atlasINT = 1;  // message ends
int AProtocol::atlasFLT = 2;   // message ends
int AProtocol::atlasSTR = 3;   // message ends
int AProtocol::atlasLST = 4;   // message ends
int AProtocol::atlasMAP = 5;   // message ends

