#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Types.h"
#include "InvisibleEntityCache.h"
#include "World.h"
#include "Connection.h"
#include "Log.h"

using namespace Time;

namespace Eris {

void InvisibleEntityCache::add(Entity *e)
{
	assert(e);
	/* if the first addition to the bucket (it's stamp) was longer ago that this time, we create a new
	bucket, instead of extending the current one. This keeps the buckets reasonably localised in
	time. */
	Time::Stamp lastUpdateBound = Time::Stamp::now() - _bucketWidthMsec;
	
	if (_buckets.empty() || (_buckets.front().stamp < lastUpdateBound)) {
		/* keep the width (times spanned by a single bucket under control : typical values would
		be a few seconds. Anything above 30 seconds is too much, if the 'view' is changing
		fast, since the buckets will get enormous (hundreds of entities) */
		
		Eris::log(LOG_DEBUG, "adding new IEC bucket, previous bucket has size %i",
			_buckets.front().contents.size());
		_buckets.push_front(_Bucket());
	}
	_buckets.front().add(e);
}	
	
void InvisibleEntityCache::flush()
{
	Time::Stamp expiry(Time::Stamp::now() - _bucketLifetimeMsec);
	while (!_buckets.empty() && (_buckets.back().stamp < expiry)) {
		for (EntitySet::iterator E=_buckets.back().contents.begin(); 
				E!=_buckets.back().contents.end();++E) {
			World::Instance()->flush(*E);
			delete *E;
		}
		Eris::log(LOG_VERBOSE, "IEC flushed %i entities", _buckets.back().contents.size());
		_buckets.pop_back();
	}
}
	
}
