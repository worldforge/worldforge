#ifndef ERIS_POLL_H
#define ERIS_POLL_H 

#include <sigc++/basic_signal.h>

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

class Poll : public SigC::Signal1<void,PollData&>
{
public:
	Poll() {}
	virtual ~Poll() {}

	virtual void addStream(const basic_socket_stream*) = 0;
	virtual void removeStream(const basic_socket_stream*) = 0;
	    
	static Poll& instance();
	static void setInstance(Poll*);

private:
	static Poll *_inst;
};

} // namespace Eris

#endif // ERIS_POLL_H
