#ifndef ERIS_POLL_GLIB_SOURCE_H
#define ERIS_POLL_GLIB_SOURCE_H

#include "PollGlibVersion.h"

namespace Eris {

class PollGlibSource
{
 public:
#ifdef ERIS_POLL_GLIB_2_0
  PollGlibSource(GMainContext *con = 0)
  {
    _source = g_source_new(&funcs_, sizeof(SourceStruct));
    ((SourceStruct*) _source)->poll = this;

    g_source_set_priority(_source, G_PRIORITY_DEFAULT_IDLE);
    _tag = g_source_attach(_source, con);
#else
  PollGlibSource()
  {
    _tag = g_source_add(G_PRIORITY_DEFAULT_IDLE, TRUE, &funcs_, this, 0, 0);
#endif
    funcs_.prepare = prepareFunc;
    funcs_.check = checkFunc;
    funcs_.dispatch = dispatchFunc;
#ifdef ERIS_POLL_GLIB_2_0
    funcs_.finalize = 0;
#else
    funcs_.destroy = 0;
#endif
  }
  virtual ~PollGlibSource()
  {
    g_source_remove(_tag);

#ifdef ERIS_POLL_GLIB_2_0
    g_source_unref(_source);
#endif
  }

 protected:

  virtual bool prepare(int& timeout) = 0;
  virtual bool check() = 0;
  virtual bool dispatch() = 0;

#ifdef ERIS_POLL_GLIB_2_0
  GSource* source() {return _source;}
#endif

 private:
#ifdef ERIS_POLL_GLIB_2_0
  GSource *_source;
#endif
  guint _tag;

  // begin funky C-like stuff

#ifdef ERIS_POLL_GLIB_2_0
  typedef struct {
    GSource source;
    PollGlibSource *poll;
  } SourceStruct;
#endif

  GSourceFuncs funcs_;

#ifdef ERIS_POLL_GLIB_2_0
  static PollGlibSource* sourceGetPoll(GSource *source)
  {
    return ((SourceStruct *) source)->poll;
  }
#else
  static PollGlibSource* sourceGetPoll(gpointer data)
  {
    return (PollGlibSource *) data;
  }
#endif

#ifdef ERIS_POLL_GLIB_2_0
  static gboolean prepareFunc(GSource *source, gint* timeout)
#else
  static gboolean prepareFunc(gpointer source, GTimeVal*, gint* timeout, gpointer)
#endif
  {
    return sourceGetPoll(source)->prepare(*timeout);
  }

#ifdef ERIS_POLL_GLIB_2_0
  static gboolean checkFunc(GSource *source)
#else
  static gboolean checkFunc(gpointer source, GTimeVal*, gpointer)
#endif
  {
    return sourceGetPoll(source)->check();
  }

#ifdef ERIS_POLL_GLIB_2_0
  static gboolean dispatchFunc(GSource *source, GSourceFunc, gpointer)
#else
  static gboolean dispatchFunc(gpointer source, GTimeVal*, gpointer)
#endif
  {
    return sourceGetPoll(source)->dispatch();
  }
};

} // namespace Eris

#endif // ERIS_POLL_GLIB_SOURCE_H
