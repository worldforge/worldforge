#ifndef ERIS_INVISIBLE_ENTITY_CACHE_H
#define ERIS_INVISIBLE_ENTITY_CACHE_H

#include <assert.h>
#include <deque>
#include <set>

#include "Time.h"
#include "Connection.h"
#include "Entity.h"

namespace Eris {

typedef std::set<Entity*> EntitySet;
	
class InvisibleEntityCache
{
public:	
	InvisibleEntityCache(unsigned long width, unsigned long life) :
		_bucketWidthMsec(width), 
		_bucketLifetimeMsec(life)
	{
	}
	
	void add(Entity *e);

	void remove(Entity *e)
	{
		assert(e);
		for (BucketQueue::iterator B=_buckets.begin(); B!=_buckets.end();++B)
			if (B->remove(e)) return;
				
		Eris::Log(LOG_ERROR, "Entity %s not found in InvisibleEntityCache doing ::remove",
			e->getID().c_str());
	}
	
	void flush();
	
protected:
	class _Bucket
	{
	public:
		void add(Entity *e)
		{
			if (contents.empty()) 
				stamp = Time::getCurrentStamp();
			contents.insert(e);
		}
		
		bool remove(Entity *e)
		{
			EntitySet::iterator C = contents.find(e);
			if (C != contents.end())
				contents.erase(C);
			return (C != contents.end());
		}
		
		Time::Stamp stamp;
		EntitySet contents;
	};

	typedef std::deque<_Bucket> BucketQueue;
	BucketQueue _buckets;
	
	unsigned long _bucketWidthMsec, 
		_bucketLifetimeMsec;
};

}

#endif