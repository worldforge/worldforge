#include <skstream/skstream.h>

#include <sigc++/object_slot.h>

#include "Types.h"

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
	PollDataDefault(const PollDefault::MapType&,
		bool&, unsigned long);

	virtual bool isReady(const basic_socket_stream*);
private:
	typedef PollDefault::MapType::const_iterator _iter;
	fd_set reading, writing;
	SOCKET_TYPE maxfd;
};

} // namespace Eris

using namespace Eris;

PollDataDefault::PollDataDefault(const PollDefault::MapType& str,
    bool &got_data, unsigned long usec_timeout) : maxfd(0)
{
	FD_ZERO(&reading);
	FD_ZERO(&writing);

#ifndef _MSC_VER
	for(_iter I = str.begin(); I != str.end(); ++I) {
#else
//MSVC stupidity fix
	std::set<const basic_socket_stream*> *str2 = const_cast<std::set<const basic_socket_stream*>* >(&str);
	for(_iter I = str2->begin(); I != str2->end(); ++I) {
#endif
		SOCKET_TYPE fd = I->first->getSocket();
		if(fd == INVALID_SOCKET)
                	continue;
		if(I->second & Poll::READ)
			FD_SET(fd, &reading);
		if(I->second & Poll::WRITE)
			FD_SET(fd, &writing);
		if(fd > maxfd)
			maxfd = fd;
	}

	if(maxfd == -1) {
		got_data = false;
		return;
	}

	struct timeval timeout = {0, usec_timeout};
	int retval = select(maxfd+1, &reading, &writing, NULL, &timeout);
	if (retval < 0)
		// FIXME - is an error from select fatal or not? At present I think yes,
		// but I'm sort of open to persuasion on this matter.
		throw InvalidOperation("Error at Meta::Poll() doing select()");

	got_data = (retval != 0);
}

bool PollDataDefault::isReady(const basic_socket_stream* str)
{
	SOCKET_TYPE fd = str->getSocket();

	return (fd != INVALID_SOCKET) && (fd <= maxfd)
		&& (FD_ISSET(fd, &reading) || FD_ISSET(fd, &writing));
}

void PollDefault::doPoll(unsigned long timeout)
{
    if(_streams.size() == 0)
	return;

    bool got_data;
    PollDataDefault data(_streams, got_data, timeout);

    if(got_data)
	emit(data);
}

void PollDefault::poll(unsigned long timeout)
{
  // This will throw if someone is using another kind
  // of poll, and that's a good thing.
  dynamic_cast<PollDefault&>(Poll::instance()).doPoll(timeout);

  Timeout::pollAll();
}

void PollDefault::addStream(const basic_socket_stream* str, Check c)
{
    assert(c && Poll::MASK);

    if(!_streams.insert(std::make_pair(str, c)).second)
	throw Eris::InvalidOperation("Duplicate streams in PollDefault"); 
}

void PollDefault::changeStream(const basic_socket_stream* str, Check c)
{
    assert(c && Poll::MASK);

    _iter i = _streams.find(str);

    if(i == _streams.end())
	throw Eris::InvalidOperation("Can't find stream in PollDefault");

    i->second = c;
}

void PollDefault::removeStream(const basic_socket_stream* str)
{
    if(_streams.erase(str) == 0)
	throw Eris::InvalidOperation("Can't find stream in PollDefault");
}
