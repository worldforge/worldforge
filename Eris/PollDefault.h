#ifndef ERIS_POLL_DEFAULT_H
#define ERIS_POLL_DEFAULT_H 

#include <set>

#include "Poll.h"

namespace Eris {

class PollDefault : public Poll, virtual public SigC::Object
{
public:
	PollDefault() {}
//	PollDefault(const SigC::Slot0<bool> &s) : _prePoll(s) {}
	virtual ~PollDefault() {}

	virtual void addStream(const basic_socket_stream*);
	virtual void removeStream(const basic_socket_stream*);

	static void poll();
private:
	std::set<const basic_socket_stream*> _streams;
	typedef std::set<const basic_socket_stream*>::iterator _iter;

	void doPoll();

//	SigC::Slot0<bool> _prePoll;
};

} // namespace Eris

#endif // ERIS_POLL_DEFAULT_H
