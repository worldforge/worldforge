
#include "Variant.h"

namespace Atlas
{

void	Variant::decref()
{
	if (--rc > 0) return;
	delete this;
}

list<VNum*>	VNum::freelist;
list<VStr*>	VStr::freelist;
list<VMap*>	VMap::freelist;
list<VVec*>	VVec::freelist;

}


