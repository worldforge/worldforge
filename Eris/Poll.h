#ifndef ERIS_POLL_H
#define ERIS_POLL_H 

#include <sigc++/signal.h>

// Forward declaration from skstream
class basic_socket_stream;

namespace Eris {

class PollData
{
public:
	PollData() {}
	virtual ~PollData() {}

	virtual bool isReady(const basic_socket_stream*) = 0;
};

class Poll
{
public:
	Poll() {}
	virtual ~Poll() {}

	enum Check {
		READ = 1,
		WRITE = 2,
		MASK = READ | WRITE
	};

	virtual void addStream(const basic_socket_stream*, Check = READ) = 0;
	virtual void changeStream(const basic_socket_stream*, Check) = 0;
	virtual void removeStream(const basic_socket_stream*) = 0;

	static Poll& instance();
	static void setInstance(Poll*);

	static void newTimeout() {new_timeout_ = true;}

    sigc::signal<void, PollData&> Ready;
private:
	Poll(const Poll&);
	static Poll *_inst;
protected:
	static bool new_timeout_;
};

} // namespace Eris

#endif // ERIS_POLL_H
