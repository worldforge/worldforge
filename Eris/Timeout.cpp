#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Types.h>
#include <Eris/Timeout.h>

#include <limits.h>

#include <Eris/Poll.h>

using namespace Time;

// The longest wait time for poll() to return.
// This should be as long as possible, since the
// only reason to limit the wait is due to
// the addition of new timeouts. The timeout sets
// a flag in the poll class if that happens,
// it's the responsibility of the poll to check.
static const unsigned long max_wait = ULONG_MAX;

namespace Eris
{

std::map<std::string, Timeout*> Timeout::_allTimeouts;
	
////////////////////////////////////////////////////////////////////////////////////////////////#	
	
Timeout::Timeout(const std::string &label, unsigned long milli) :
	_label(label),
	_fired(false)
{
	TimeoutMap::iterator T = _allTimeouts.find(label);
	if (T != _allTimeouts.end())
	    throw InvalidOperation("Duplicate label '" + label + "' for timeout");
	
	_allTimeouts.insert(_allTimeouts.begin(),
	    TimeoutMap::value_type(label, this));
	
	_due = Time::Stamp::now() + milli;

	Poll::newTimeout();
}

Timeout::~Timeout()
{
	TimeoutMap::iterator T = _allTimeouts.find(_label);
	if (T == _allTimeouts.end())
		throw InvalidOperation("Corrupted timeout map - very bad!");
	
	_allTimeouts.erase(T);
}

/*
Timeout& Timeout::operator=(const Timeout &t)
{
	
}
*/

bool Timeout::isExpired() const
{
	return (_due < Time::Stamp::now());
}

unsigned long Timeout::poll(const Time::Stamp &t)
{
	if (!_fired) {
		long diff = _due - t;
		if(diff > 0) // not finished yet
			return diff;
		Expired();	// invoke the signal
		_fired = true;
	}

	return max_wait; // Doesn't need to be called again
}

void Timeout::reset(unsigned long milli)
{
	_fired = false;
	_due = Time::Stamp::now() + milli;
}

void Timeout::extend(unsigned long milli)
{
	_due = _due + milli;
}

//////////////////////////////////////////////////////////////////////////////////////

const Timeout* Timeout::findByName(const std::string &nm)
{
	TimeoutMap::iterator T = _allTimeouts.find(nm);
	if (T == _allTimeouts.end())
		return NULL;
	return T->second;
}

unsigned long Timeout::pollAll()
{
	Time::Stamp now = Time::Stamp::now();
	unsigned long wait = max_wait;
	for (TimeoutMap::iterator T=_allTimeouts.begin(); T != _allTimeouts.end(); ++T) {
		unsigned long this_wait = T->second->poll(now);
		if(this_wait < wait)
			wait = this_wait;
	}
	return wait;
}


}
