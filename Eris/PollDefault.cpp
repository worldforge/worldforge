#include <Eris/Types.h>

#include <Eris/PollDefault.h>
#include <Eris/Exceptions.h>
#include <Eris/DeleteLater.h>
#include <Eris/Log.h>
#include <Eris/TimedEventService.h>

#include <skstream/skstream.h>

bool Eris::Poll::new_timeout_ = false;

Eris::Poll& Eris::Poll::instance()
{
  if(!_inst)
    _inst = new PollDefault();

  return *_inst;
}

void Eris::Poll::setInstance(Poll* p)
{
  if(_inst)
    throw InvalidOperation("Can't set poll instance, already have one");

  _inst = p;
}

Eris::Poll* Eris::Poll::_inst = 0;

namespace Eris {

class PollDataDefault : public PollData
{
public:
	PollDataDefault(const PollDefault::MapType&, bool&, unsigned long);

	virtual bool isReady(const basic_socket*);
private:
	typedef PollDefault::MapType::const_iterator _iter;
	fd_set reading, writing, exceptions;
	SOCKET_TYPE maxfd;
};

} // namespace Eris

using namespace Eris;

PollDataDefault::PollDataDefault(const PollDefault::MapType& str,
    bool &got_data, unsigned long msec_timeout) : maxfd(0)
{
	FD_ZERO(&reading);
	FD_ZERO(&writing);
	FD_ZERO(&exceptions);
	got_data = false;

#ifndef _NOT_MSC_VER
	for(PollDefault::MapType::const_iterator I = str.begin(); I != str.end(); ++I) {
#else
//MSVC stupidity fix
	std::map<const basic_socket_stream*, Check> *str2 = const_cast<std::map<const basic_socket_stream*, Check>* >(&str);
	for(_iter I = str2->begin(); I != str2->end(); ++I) {
#endif
		SOCKET_TYPE fd = I->first->getSocket();
		if(fd == INVALID_SOCKET) continue;
        
		got_data = true;
		if(I->second & Poll::READ)
			FD_SET(fd, &reading);
		if(I->second & Poll::WRITE)
			FD_SET(fd, &writing);
		if(I->second & Poll::EXCEPT)
			FD_SET(fd, &exceptions);
		if(fd > maxfd)
			maxfd = fd;
	}

	if (!got_data) return;

	struct timeval timeout = {msec_timeout / 1000, (msec_timeout % 1000) * 1000};
	int retval = select(maxfd+1, &reading, &writing, &exceptions, &timeout);
	if (retval < 0) {
        warning() << "select() returned error: " << retval;
        got_data = false;
    }

	got_data = (retval != 0);
}

bool PollDataDefault::isReady(const basic_socket* str)
{
	SOCKET_TYPE fd = str->getSocket();

	return (fd != INVALID_SOCKET) && (fd <= maxfd)
		&& (FD_ISSET(fd, &reading) ||
                    FD_ISSET(fd, &writing) ||
                    FD_ISSET(fd, &exceptions));
}

void PollDefault::doPoll(unsigned long timeout)
{
    if(_streams.size() == 0)
	return;

    bool got_data;
    PollDataDefault data(_streams, got_data, timeout);

    if(got_data)
	Ready.emit(data);
}

void PollDefault::poll(unsigned long timeout)
{
  // This will throw if someone is using another kind
  // of poll, and that's a good thing.
  PollDefault &inst = dynamic_cast<PollDefault&>(Poll::instance());

  // Prevent reentrancy
  static bool already_polling = false;
  assert(!already_polling);
  already_polling = true;

  try {
    unsigned long wait_time = 0;
    inst.new_timeout_ = false;

    // This will only happen for timeout != 0
    while(wait_time < timeout) {
      inst.doPoll(wait_time);
      timeout -= wait_time;
      wait_time = TimedEventService::instance()->tick();
      if(inst.new_timeout_) {
        // Added a timeout, the time until it must be called
        // may be shorter than wait_time
        wait_time = 0;
        inst.new_timeout_ = false;
      }
    }

    inst.doPoll(timeout);
    TimedEventService::instance()->tick(true);
    execDeleteLaters();

    // We're done, turn off the reentrancy prevention flag
    assert(already_polling);
    already_polling = false;
  }
  catch(...) {
    already_polling = false;
    throw;
  }
}

int PollDefault::maxStreams() const
{
    return FD_SETSIZE;
}

int PollDefault::maxConnectingStreams() const
{
#ifdef __WIN32__
#else // __WIN32__
    return maxStreams();
#endif // __WIN32__
}

void PollDefault::addStream(const basic_socket* str, Check c)
{
    assert(c && Poll::MASK);

    if(!_streams.insert(std::make_pair(str, c)).second)
	throw Eris::InvalidOperation("Duplicate streams in PollDefault"); 
}

void PollDefault::changeStream(const basic_socket* str, Check c)
{
    assert(c && Poll::MASK);

    _iter i = _streams.find(str);

    if(i == _streams.end())
	throw Eris::InvalidOperation("Can't find stream in PollDefault");

    i->second = c;
}

void PollDefault::removeStream(const basic_socket* str)
{
    if(_streams.erase(str) == 0)
	throw Eris::InvalidOperation("Can't find stream in PollDefault");
}
