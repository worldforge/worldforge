#ifndef ERIS_POLL_H
#define ERIS_POLL_H 

#include <sigc++/signal.h>

namespace Eris {

class Poll
{
public:
	Poll() {}
	virtual ~Poll() {}

	static const int
		READ = 1,
		WRITE = 2,
		EXCEPT = 4,
		MASK = READ | WRITE | EXCEPT;
	typedef int Check;

	virtual int maxStreams() const = 0;
	virtual int maxConnectingStreams() const = 0;

	static Poll& instance();
	static void setInstance(Poll*);

	static void newTimedEvent() {new_timeout_ = true;}

private:
	Poll(const Poll&);
	static Poll *_inst;
protected:
	static bool new_timeout_;
};

} // namespace Eris

#endif // ERIS_POLL_H
