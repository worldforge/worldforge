#include <skstream.h>

#include <sigc++/object_slot.h>

#include "Eris/Types.h"

#include "PollDefault.h"
#include "Timeout.h"
#include "Log.h"

Eris::Poll& Eris::Poll::instance()
{
  if(!_inst)
    _inst = new PollDefault();

  return *_inst;
}

void Eris::Poll::setInstance(Poll* p)
{
  if(_inst)
    throw InvalidOperation("Can't set Poll instance, already have one");

  _inst = p;
}

Eris::Poll* Eris::Poll::_inst = 0;

namespace Eris {

class PollDataDefault : public PollData
{
public:
	PollDataDefault(const std::set<const basic_socket_stream*>&, bool&);

	virtual bool isReady(const basic_socket_stream*);
private:
	typedef std::set<const basic_socket_stream*>::iterator _iter;
	fd_set pending;
	SOCKET_TYPE maxfd;
};

} // namespace Eris

using namespace Eris;

PollDataDefault::PollDataDefault(const std::set<const basic_socket_stream*>& str,
				 bool &got_data) : maxfd(0)
{
	FD_ZERO(&pending);

	for(_iter I = str.begin(); I != str.end(); ++I) {
		SOCKET_TYPE fd = (*I)->getSocket();
		FD_SET(fd, &pending);
		if(fd > maxfd)
			maxfd = fd;
	}

	if(maxfd == 0) {
		got_data = false;
		return;
	}

	struct timeval TIMEOUT_ZERO = {0, 0};
	int retval = select(maxfd+1, &pending, NULL, NULL, &TIMEOUT_ZERO);
	if (retval < 0)
		// FIXME - is an error from select fatal or not? At present I think yes,
		// but I'm sort of open to persuasion on this matter.
		throw InvalidOperation("Error at Meta::Poll() doing select()");

	got_data = (retval != 0);
}

bool PollDataDefault::isReady(const basic_socket_stream* str)
{
	SOCKET_TYPE fd = str->getSocket();

	return (fd <= maxfd) && FD_ISSET(fd, &pending);
}

void PollDefault::doPoll()
{
    if(_streams.size() == 0)
	return;

    bool got_data;
    PollDataDefault data(_streams, got_data);

    if(got_data)
	emit(data);
}

void PollDefault::poll()
{
  // This will throw if someone is using another kind
  // of poll, and that's a good thing.
  dynamic_cast<PollDefault&>(Poll::instance()).doPoll();

  Timeout::pollAll();
}

void PollDefault::addStream(const basic_socket_stream* str)
{
	if(!_streams.insert(str).second)
		throw Eris::InvalidOperation("Duplicate streams in PollDefault"); 
}

void PollDefault::removeStream(const basic_socket_stream* str)
{
    if(_streams.erase(str) == 0)
	throw Eris::InvalidOperation("Can't find stream in PollDefault");
}
