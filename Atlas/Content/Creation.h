/*
        AtlasFactory.h
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#ifndef __AtlasFactory_h_
#define __AtlasFactory_h_

#include <string>
using namespace std;

#include <AtlasObject.h>
#include "Arg.h"

AObject createOperation(const string& id, long serialno, Arg* args ...);
AObject createEntity(Arg* args ...);

#endif
