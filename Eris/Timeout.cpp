#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Types.h"
#include "Timeout.h"

using namespace Time;

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
	
	_due = Time::getCurrentStamp() + milli;
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
	return (_due < Time::getCurrentStamp());
}

void Timeout::poll(const Time::Stamp &t)
{
	if (!_fired && (_due < t)) {
		Expired();	// invoke the signal
		_fired = true;
	}
}

void Timeout::reset(unsigned long milli)
{
	_fired = false;
	_due = Time::getCurrentStamp() + milli;
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

void Timeout::pollAll()
{
	Time::Stamp now = Time::getCurrentStamp();
	for (TimeoutMap::iterator T=_allTimeouts.begin(); T != _allTimeouts.end(); ++T) {
		T->second->poll(now);
	}
}

}