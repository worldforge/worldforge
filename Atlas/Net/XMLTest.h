#include "Object.h"

#ifndef __XMLTest_h_
#define __XMLTest_h_

#include "Transport.h"

class XMLTest
{

public:

void walkTree(int nest, string name, const AObject& list);
void DisplayMessage(const AObject& msg);
void execute();

};

#endif




