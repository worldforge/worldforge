
#include "Variant.h"

namespace Atlas
{

Variant::Variant()	{ rc=1; rt = None; }
Variant::~Variant()	{}

void	Variant::decref()
{
	if (--rc > 0) return;
	delete this;
}

std::list<VNum*>	VNum::freelist;
std::list<VStr*>	VStr::freelist;
std::list<VMap*>	VMap::freelist;
std::list<VVec*>	VVec::freelist;

}


