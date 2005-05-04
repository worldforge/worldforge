#ifndef ERIS_POLL_GLIB_FD_H
#define ERIS_POLL_GLIB_FD_H

#include <skstream/skstream.h>

namespace Eris
{

class PollGlibFD
{
public:
  PollGlibFD(GSource *source, const basic_socket_stream *str, gushort events)
	: _source(source)
  {
    _fd.fd = str->getSocket();
    _fd.events = events;
    _fd.revents = 0;
    g_source_add_poll(source, &_fd);
  }
  
  ~PollGlibFD()
  {
    g_source_remove_poll(_source, &_fd);
  }

  gboolean check() { return (_fd.events & _fd.revents) != 0;}
  void setEvents(gushort events) {_fd.events = events;}
  gushort getEvents(gushort events) {return _fd.events;}
private:
  GPollFD _fd;
  GSource *_source;
};

} // namespace Eris

#endif // ERIS_POLL_GLIB_FD_H
