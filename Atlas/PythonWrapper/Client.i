%typemap(python,default) Filter* {
        $target=0;
}

class Client
{
public:
    Client( Socket*, Codec*, Filter*);
    virtual ~Client();

    ///poll client stream. Will read and write from socket stream if data available
    void    doPoll();
    ///send message (msg->codec->filter->socket)
    void    sendMsg( const Object& msg );
    ///get message  (socket->filter->codec->msg)
    //CHEAT!: does typemap work here?
    void    readMsg( Object& msg );
    ///change Codec used by client
    void    setCodec(Codec* acodec) { codec = acodec; }
    //change Filter used by client
    void    setFilter(Filter* aFilter ) { filter = aFilter; }
    ///return socket this client is using
    SOCKET  getSock();
    void	chkMsgs();

    bool    canRead();
    bool    canSend() const { return true; }

    bool    gotErrs();
};


%addmethods Client {
    ///to be overidden in subclasses. Called when doPoll() recieves a new object
    virtual void    gotMsg(const Object& msg)
    {
    }
    ///to be overidden in subclesses. Called on a socket error/disconnect
    virtual void    gotDisconnect()
    {
    }
};
