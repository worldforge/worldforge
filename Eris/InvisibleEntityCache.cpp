#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Types.h"
#include "InvisibleEntityCache.h"
#include "World.h"

using namespace Time;

namespace Eris {

void InvisibleEntityCache::add(Entity *e)
{
	assert(e);
	if (_buckets.front().stamp < (Time::getCurrentStamp() - _bucketWidthMsec))
		/* keep the width (times spanned by a single bucket under control : typical values would
		be a few seconds. Anything above 30 seconds is too much, if the 'view' is changing
		fast, since the buckets will get enormous (hundreds of entities) */
		_buckets.push_front(_Bucket());
	_buckets.front().add(e);
}	
	
void InvisibleEntityCache::flush()
{
	Time::Stamp expiry(Time::getCurrentStamp() - _bucketLifetimeMsec);
	while (_buckets.back().stamp < expiry) {
		for (EntitySet::iterator E=_buckets.back().contents.begin(); 
				E!=_buckets.back().contents.end();++E) {
			World::Instance()->flush(*E);
			delete *E;
		}
		_buckets.pop_back();
	}
}
	
}