#ifndef ERIS_TIMEOUT_H
#define ERIS_TIMEOUT_H

#include "Types.h"
#include "Timestamp.h"

namespace Eris {

/** Timeout  */	
class Timeout : public SigC::Object
{
public:	
	/** standard constructor for new timeouts
	@param label The unique timeout identification label
	@param milli The initial timeout  value, in milliseconds */
	Timeout(const std::string &label, unsigned long milli);
	Timeout();
	~Timeout();

	/// assignment operator
	Timeout& operator=(const Timeout &t);

	/** extend the timeout termination by the specified time. This has no effect if the timeout
	has already fired
	@param msec The interval to extend the timeout by, in milliseconds */
	void extend(unsigned long msec);

	/** reset the timeout to the specified time, and reset the 'expired' flag. This means invoking reset()
	is equivalent to destroying the Timeout object and creating a new one with the same name. */
	void reset(unsigned long msec);
	
	/// cancel the Timeout immediately
	void cancel();
	unsigned long poll(const Time::Stamp &t);
	
	/// test whether or not the timeout has fired or not
	bool isExpired() const;

	/** The timeout signal */
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
	static unsigned long pollAll();

	/** helper function to lookup a timeout and call extend(). If the timeout is not found,
	InvalidOperation will be thrown */
	static void extendByName(const std::string &lbl, unsigned long msec);
	
	/** helper function to lookup a timeout and call cancel(). As above, a missing timeout will
	cause an InvalidOperation to be thrown. */
	static void cancelByName(const std::string &lbl);

protected:
	const std::string _label;	///< the unique label of the timeout
	Time::Stamp _due;			///< interval at which the timeout is due
	bool _fired;				///< flag if the timeout has expired

	typedef std::map<std::string, Timeout*> TimeoutMap;
	static TimeoutMap _allTimeouts;

private:
	/** disable the copy constructor, since it is harmful for Timeouts (_label must be
	unique in the _allTimeouts map) */
	Timeout(const Timeout &t);
};

} // of Eris namespace

#endif
