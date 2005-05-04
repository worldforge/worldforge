#ifndef ERIS_POLL_GLIB_SOURCE_H
#define ERIS_POLL_GLIB_SOURCE_H

namespace Eris {

class PollGlibSource
{
 public:
  PollGlibSource(GMainContext *con = 0)
  {
    _source = g_source_new(&funcs_, sizeof(SourceStruct));
    ((SourceStruct*) _source)->poll = this;

    g_source_set_priority(_source, G_PRIORITY_DEFAULT_IDLE);
    _tag = g_source_attach(_source, con);
    funcs_.prepare = prepareFunc;
    funcs_.check = checkFunc;
    funcs_.dispatch = dispatchFunc;
    funcs_.finalize = 0;
  }
  virtual ~PollGlibSource()
  {
    g_source_remove(_tag);

    g_source_unref(_source);
  }

 protected:

  virtual bool prepare(int& timeout) = 0;
  virtual bool check() = 0;
  virtual bool dispatch() = 0;

  GSource* source() {return _source;}

 private:
  GSource *_source;
  guint _tag;

  // begin funky C-like stuff

  typedef struct {
    GSource source;
    PollGlibSource *poll;
  } SourceStruct;

  GSourceFuncs funcs_;

  static PollGlibSource* sourceGetPoll(GSource *source)
  {
    return ((SourceStruct *) source)->poll;
  }

  static gboolean prepareFunc(GSource *source, gint* timeout)
  {
    return sourceGetPoll(source)->prepare(*timeout);
  }

  static gboolean checkFunc(GSource *source)
  {
    return sourceGetPoll(source)->check();
  }

  static gboolean dispatchFunc(GSource *source, GSourceFunc, gpointer)
  {
    return sourceGetPoll(source)->dispatch();
  }
};

} // namespace Eris

#endif // ERIS_POLL_GLIB_SOURCE_H
