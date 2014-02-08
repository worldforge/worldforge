#ifndef ERIS_POLL_GLIB_H
#define ERIS_POLL_GLIB_H

#include <map>

#include <Eris/DeleteLater.h>
#include <Eris/Exceptions.h>
#include <Eris/Poll.h>
#include <Eris/PollGlibFD.h>
#include <Eris/PollGlibSource.h>
#include <Eris/Types.h>

namespace Eris
{

class PollGlib : public Eris::Poll, public Eris::PollData, public PollGlibSource
{
public:
	PollGlib(GMainContext * pMainContext = 0) :
		PollGlibSource(pMainContext),
		_wait_time(-1)
	{
		g_timeout_add(250, (GSourceFunc)(bTimeoutCallback), this);
	}
	
	static gboolean bTimeoutCallback(PollGlib * pPoller)
	{
		pPoller->_wait_time = Eris::TimedEventService::instance()->tick();
		execDeleteLaters();
		
		return TRUE;
	}
  
  virtual ~PollGlib()
  {
    for(StreamMap::iterator I = _streams.begin(); I != _streams.end(); ++I)
      delete I->second;
  }

  virtual void addStream(const basic_socket_stream* str, Check check)
  {
    if(!(check & MASK))
      throw Eris::InvalidOperation("Null check in PollGlib");

    gushort events = getEvents(check);

    PollGlibFD* fd = new PollGlibFD(source(), str, events);

    if(!_streams.insert(StreamMap::value_type(str, fd)).second) {
      delete fd;
      throw Eris::InvalidOperation("Duplicate streams in PollGlib");
    }
  }

  virtual void changeStream(const basic_socket_stream* str, Check check)
  {
    if(!(check & MASK))
      throw Eris::InvalidOperation("Null check in PollGlib");

    StreamMap::iterator I = _streams.find(str);

    if(I == _streams.end())
      throw Eris::InvalidOperation("Can't find stream in PollGlib");

    I->second->setEvents(getEvents(check));
  }

  virtual void removeStream(const basic_socket_stream* str)
  {
    StreamMap::iterator I = _streams.find(str);

    if(I == _streams.end())
      throw Eris::InvalidOperation("Can't find stream in PollGlib");

    PollGlibFD *data = I->second;

    _streams.erase(I);

    delete data;
  }

  virtual bool isReady(const basic_socket_stream* str)
  {
    StreamMap::iterator I = _streams.find(str);

    return I != _streams.end() && I->second->check();
  }

protected:
  virtual bool prepare(int& timeout)
  {
    timeout = _wait_time;
    return timeout != -1;
  }
  virtual bool check()
  {
    for(StreamMap::iterator I = _streams.begin(); I != _streams.end(); ++I)
      if(I->second->check())
        return TRUE;

    return FALSE;
  }
  virtual bool dispatch()
  {
    if(check())
      Ready.emit(*this);

    unsigned long wait = Eris::TimedEventService::instance()->tick();

    if(new_timeout_) {
      _wait_time = 0;
       new_timeout_ = false;
    }
    else if(wait <= G_MAXINT)
      _wait_time = wait;
    else
      _wait_time = -1;

    return TRUE;
  }

 private:
  static gushort getEvents(Eris::Poll::Check check)
  {
    assert(check & Eris::Poll::MASK);

    gushort events = 0;

    if(check & Eris::Poll::READ)
      events |= G_IO_IN;
    if(check & Eris::Poll::WRITE)
      events |= G_IO_OUT;

    assert(events);

    return events;
  }

  typedef std::map<const basic_socket_stream*,PollGlibFD*> StreamMap;
  StreamMap _streams;

  gint _wait_time;
};

} // namespace Eris

#endif // ERIS_POLL_GLIB_H
