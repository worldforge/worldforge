#ifndef ERIS_POLL_DEFAULT_H
#define ERIS_POLL_DEFAULT_H 

#include <map>

#include <Eris/Poll.h>

namespace Eris {

class PollDefault : public Poll, virtual public SigC::Object
{
public:
	PollDefault() {}
//	pollDefault(const SigC::Slot0<bool> &s) : _prePoll(s) {}
	virtual ~PollDefault() {}

	virtual void addStream(const basic_socket_stream*, Check);
	virtual void changeStream(const basic_socket_stream*, Check);
	virtual void removeStream(const basic_socket_stream*);

	static void poll(unsigned long timeout = 0);

	typedef std::map<const basic_socket_stream*,Check> MapType;
private:
	MapType _streams;
	typedef MapType::iterator _iter;

	void doPoll(unsigned long timeout);

//	SigC::Slot0<bool> _prepoll;
};

} // namespace Eris

#endif // ERIS_POLL_DEFAULT_H
