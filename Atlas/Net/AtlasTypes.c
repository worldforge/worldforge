/*
        AtlasTypes.c
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasTypes.h"

void initAtlasTypes()
{
	Py_Initialize();

	initURI();
	initURIList();
	initIntList();
	initLongList();
	initFloatList();
	initStringList();
}
