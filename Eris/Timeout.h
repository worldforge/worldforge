#ifndef ERIS_TIMEOUT_H
#define ERIS_TIMEOUT_H

#include "Types.h"
#include "Time.h"

namespace Eris {
	
class Timeout : public SigC::Object
{
public:	
	Timeout(const std::string &label, unsigned long milli);
	Timeout();
	~Timeout();

	/// assignment operator
	Timeout& operator=(const Timeout &t);

	void extend(unsigned long msec);
	void reset(unsigned long msec);
	
	void cancel();
	void poll(const Time::Stamp &t);
	
	bool isExpired() const;

	SigC::Signal0<void> Expired;

	///////////////////////////////////////////////////////////////////////////////

	/** Access an existing timeout by name. This will throw an exception
	if no such timeout exists. Also note that it returns a reference, so don't
	mis-treat the poor thing */
	static const Timeout* findByName(const std::string &lbl);

	/** poll all running timeouts. This calls poll() on each timeout, and hence
	triggers expiry signals and so on. Timeout accuracy is directly related
	to how frequently polling is done (but timeouts aren't supposed to be
	accurate, so once per main loop iteration should be fine) */
	static void pollAll();

	static void extendByName(const std::string &lbl, unsigned long msec);
	static void cancelByName(const std::string &lbl);

protected:
	const std::string _label;
	Time::Stamp _due;
	bool _fired;

	typedef std::map<std::string, Timeout*> TimeoutMap;
	static TimeoutMap _allTimeouts;

private:
	// disable the copy constructor
	Timeout(const Timeout &t);
};

} // of Eris namespace

#endif