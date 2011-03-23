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
//	PollDefault(const sigc::slot<bool> &s) : _prePoll(s) {}
	virtual ~PollDefault() {}

	virtual void addStream(const basic_socket*, Check);
	virtual void changeStream(const basic_socket*, Check);
	virtual void removeStream(const basic_socket*);

	static void poll(unsigned long timeout = 0);

	typedef std::map<const basic_socket*,Check> MapType;
private:
	MapType _streams;
	typedef MapType::iterator _iter;

	void doPoll(unsigned long timeout);

//	sigc::slot<bool> _prePoll;
};

} // namespace Eris

#endif // ERIS_POLL_DEFAULT_H
