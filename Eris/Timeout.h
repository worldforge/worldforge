#ifndef ERIS_TIMEOUT_H
#define ERIS_TIMEOUT_H

#include <wfmath/timestamp.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

namespace Eris {

/** Timeout  */
class Timeout : virtual public sigc::trackable
{
public:

	/** standard constructor for new timeouts
	@param label The unique timeout identification label
	@param milli The initial timeout  value, in milliseconds */
	Timeout(unsigned long milli);

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

        /** Check if the Timeout has expired, and if so fire it's signal.
        @return The time remaining until the timeout will expire.
        */
	WFMath::TimeDiff poll(const WFMath::TimeStamp &t);

	/// test whether or not the timeout has fired or not
	bool isExpired() const;

	/** The timeout signal */
	sigc::signal<void> Expired;

	/** poll all running timeouts. This calls poll() on each timeout, and hence
	triggers expiry signals and so on. Timeout accuracy is directly related
	to how frequently polling is done (but timeouts aren't supposed to be
	accurate, so once per main loop iteration should be fine) */
	static unsigned long pollAll();

protected:
    WFMath::TimeStamp _due;			///< interval at which the timeout is due
	bool _fired;				///< flag if the timeout has expired

private:
	/** disable the copy constructor */
	Timeout(const Timeout &t);
};

} // of Eris namespace

#endif
