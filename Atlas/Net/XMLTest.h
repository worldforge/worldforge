#include "AtlasObject.h"

#ifndef __XMLTest_h_
#define __XMLTest_h_

#include "AtlasDebug.h"
#include "AtlasCodec.h"
#include "AtlasObject.h"
#include "AtlasProtocol.h"
#include "AtlasXMLProtocol.h"

class XMLTest
{

public:

void walkTree(int nest, string name, const AObject& list);
void DisplayMessage(const AObject& msg);
void execute();

};

#endif




