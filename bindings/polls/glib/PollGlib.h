#ifndef ERIS_POLL_GLIB_H
#define ERIS_POLL_GLIB_H

#include <Eris/pollGlibVersion.h>
#include <Eris/pollGlibFD.h>
#include <Eris/pollGlibSource.h>

#include <map>
#include <Eris/poll.h>
#include <Eris/Types.h>
#include <Eris/Timeout.h>

namespace Eris {

class pollGlib : public Eris::Poll, public Eris::PollData, public PollGlibSource
{
public:
#ifdef ERIS_POLL_GLIB_2_0
  pollGlib(GMainContext *con = 0) : PollGlibSource(con), _wait_time(0) {}
#else
  pollGlib() : _wait_time(0) {}
#endif
  virtual ~pollGlib()
  {
    for(StreamMap::iterator I = _streams.begin(); I != _streams.end(); ++I)
      delete I->second;
  }

  virtual void addStream(const basic_socket_stream* str, Check check)
  {
    if(!(check & MASK))
      throw Eris::InvalidOperation("Null check in pollGlib");

    gushort events = getEvents(check);

#ifdef ERIS_POLL_GLIB_2_0
    pollGlibFD* fd = new PollGlibFD(source(), str, events);
#else
    pollGlibFD* fd = new PollGlibFD(str, events);
#endif

    if(!_streams.insert(StreamMap::value_type(str, fd)).second) {
      delete fd;
      throw Eris::InvalidOperation("Duplicate streams in pollGlib");
    }
  }

  virtual void changeStream(const basic_socket_stream* str, Check check)
  {
    if(!(check & MASK))
      throw Eris::InvalidOperation("Null check in pollGlib");

    StreamMap::iterator I = _streams.find(str);

    if(I == _streams.end())
      throw Eris::InvalidOperation("Can't find stream in pollGlib");

    I->second->setEvents(getEvents(check));
  }

  virtual void removeStream(const basic_socket_stream* str)
  {
    StreamMap::iterator I = _streams.find(str);

    if(I == _streams.end())
      throw Eris::InvalidOperation("Can't find stream in pollGlib");

    pollGlibFD *data = I->second;

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
      emit(*this);

    unsigned long wait = Eris::Timeout::pollAll();

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
  static gushort getEvents(Eris::poll::Check check)
  {
    assert(check & Eris::poll::MASK);

    gushort events = 0;

    if(check & Eris::poll::READ)
      events |= G_IO_IN;
    if(check & Eris::poll::WRITE)
      events |= G_IO_OUT;

    assert(events);

    return events;
  }

  typedef std::map<const basic_socket_stream*,pollGlibFD*> StreamMap;
  StreamMap _streams;

  gint _wait_time;
};

} // namespace Eris

#endif // ERIS_POLL_GLIB_H
