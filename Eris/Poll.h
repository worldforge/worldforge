#ifndef ERIS_POLL_H
#define ERIS_POLL_H 

#include <sigc++/signal.h>

// Forward declaration from skstream
class basic_socket;

namespace Eris {

class PollData
{
public:
	PollData() {}
	virtual ~PollData() {}

	virtual bool isReady(const basic_socket*) = 0;
};

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

	virtual void addStream(const basic_socket*, Check = READ) = 0;
	virtual void changeStream(const basic_socket*, Check) = 0;
	virtual void removeStream(const basic_socket*) = 0;

	static Poll& instance();
	static void setInstance(Poll*);

	static void newTimedEvent() {new_timeout_ = true;}

    sigc::signal<void, PollData&> Ready;
private:
	Poll(const Poll&);
	static Poll *_inst;
protected:
	static bool new_timeout_;
};

} // namespace Eris

#endif // ERIS_POLL_H
