#ifndef ERIS_POLL_DEFAULT_H
#define ERIS_POLL_DEFAULT_H 

#include <Eris/Poll.h>

#include <sigc++/trackable.h>

#include <map>

namespace Eris {

class PollDefault : public Poll, virtual public sigc::trackable
{
public:
	PollDefault() {}
	virtual ~PollDefault() {}

	virtual int maxStreams() const;
	virtual int maxConnectingStreams() const;

	static void poll(unsigned long timeout = 0);

private:
	void doPoll(unsigned long timeout);

};

} // namespace Eris

#endif // ERIS_POLL_DEFAULT_H
