/*
        AtlasProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Protocol.h"

string	AProtocol::prefix("");

int AProtocol::atlasERRTOK = -1;  // error token
int AProtocol::atlasMSGBEG = 1;   // message begins
int AProtocol::atlasATRBEG = 2;   // attribute begins
int AProtocol::atlasATRVAL = 3;   // attribute value
int AProtocol::atlasATREND = 4;   // attribute end
int AProtocol::atlasMSGEND = 5;   // message ends

int AProtocol::atlasINT		= 1;
int AProtocol::atlasLNG		= 2;
int AProtocol::atlasFLT		= 3;
int AProtocol::atlasSTR		= 4;
int AProtocol::atlasURI		= 5;
int AProtocol::atlasLSTINT	= 6;
int AProtocol::atlasLSTLNG	= 7;
int AProtocol::atlasLSTFLT	= 8;
int AProtocol::atlasLSTSTR	= 9;
int AProtocol::atlasLSTURI	= 10;
int AProtocol::atlasLST		= 11;
int AProtocol::atlasMAP		= 12;

