#ifndef ERIS_POLL_GLIB_FD_H
#define ERIS_POLL_GLIB_FD_H

#include <Eris/PollGlibVersion.h>
#include <skstream/skstream.h>

namespace Eris {

class PollGlibFD {
public:
#ifdef ERIS_POLL_GLIB_2_0
  PollGlibFD(GSource *source, const basic_socket_stream *str, gushort events)
	: _source(source)
#else
  PollGlibFD(const basic_socket_stream *str, gushort events)
#endif
  {
    _fd.fd = str->getSocket();
    _fd.events = events;
    _fd.revents = 0;
#ifdef ERIS_POLL_GLIB_2_0
    g_source_add_poll(source, &_fd);
#else
    g_main_add_poll(&_fd, G_PRIORITY_DEFAULT);
#endif
  }
  ~PollGlibFD()
  {
#ifdef ERIS_POLL_GLIB_2_0
    g_source_remove_poll(_source, &_fd);
#else
    g_main_remove_poll(&_fd);
#endif
  }

  gboolean check() { return (_fd.events & _fd.revents) != 0;}
  void setEvents(gushort events) {_fd.events = events;}
  gushort getEvents(gushort events) {return _fd.events;}
private:
  GPollFD _fd;
#ifdef ERIS_POLL_GLIB_2_0
  GSource *_source;
#endif
};

} // namespace Eris

#endif // ERIS_POLL_GLIB_FD_H
