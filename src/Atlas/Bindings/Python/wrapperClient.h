#include <Atlas/Net/Client.h>
#include <Python.h>

#ifdef SWIG
%typemap(python,default) Filter* {
        $target=0;
}

%typemap(python,in) PyObject *instance {
  $target = $source;
}


class wrapperClient {
#else
using namespace Atlas;
class wrapperClient : public Client {
#endif
public:
  wrapperClient( Socket* s, Codec* c, Filter* f) : Client(s,c,f) {}
#ifdef SWIG
    virtual ~wrapperClient();

    ///poll client stream. Will read and write from socket stream if data available
    void    doPoll();
    ///send message (msg->codec->filter->socket)
    void    sendMsg( const Object& msg );
    ///get message  (socket->filter->codec->msg)
    void    readMsg( Object& msg );
    ///change Codec used by client
    void    setCodec(Codec* acodec) { codec = acodec; }
    //change Filter used by client
    void    setFilter(Filter* aFilter ) { filter = aFilter; }
#endif
    ///to be overidden in subclasses. Called when doPoll() recieves a new object
    virtual void    gotMsg(const Object& msg);
    ///to be overidden in subclesses. Called on a socket error/disconnect
    virtual void    gotDisconnect();
    void setPythonInstance(PyObject *instance);
#ifdef SWIG
    ///return socket this client is using
    SOCKET  getSock();
    void	chkMsgs();

    bool    canRead();
    bool    canSend() const { return true; }

    bool    gotErrs();
#endif
private:
    PyObject *m_instance;
};
