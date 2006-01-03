#ifndef ERIS_TIMEOUT_H
#define ERIS_TIMEOUT_H

#include <Eris/TimedEventService.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

namespace Eris {

/** Timeout  */
class Timeout : public TimedEvent, virtual public sigc::trackable
{
public:

	/**
    @brief standard constructor for new timeouts
	@param label The unique timeout identification label
	@param milli The initial timeout  value, in milliseconds
    */
	Timeout(unsigned long milli);

	virtual ~Timeout();

	/// assignment operator
	Timeout& operator=(const Timeout &t);

	/** 
    @brief Extend the timeout termination by the specified time. 
    This has no effect if the timeout has already fired
	
    @param msec The interval to extend the timeout by, in milliseconds
    */
	void extend(unsigned long msec);

	/**
    @brief reset the timeout to the specified time, and reset the 'expired' flag. 
    This means invoking reset() is equivalent to destroying the Timeout object
    and creating a new one with the same name.
    */
	void reset(unsigned long msec);

	/// cancel the Timeout immediately
	void cancel();
    
	/// test whether or not the timeout has fired or not
	bool isExpired() const;

	/** The timeout signal */
	sigc::signal<void> Expired;

    virtual void expired();
    
    virtual const WFMath::TimeStamp& due() const
    {
        return _due;
    }
protected:
    WFMath::TimeStamp _due;			///< interval at which the timeout is due
	bool _fired;				///< flag if the timeout has expired

private:
	/** disable the copy constructor */
	Timeout(const Timeout &t);
};

} // of Eris namespace

#endif
