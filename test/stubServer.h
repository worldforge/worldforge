#ifndef ERIS_STUB_SERVER_H
#define ERIS_STUB_SERVER_H

#include <skstream.h>
#include <skserver.h>

#include <queue>

#include "Atlas/Message/DecoderBase.h"
#include <Atlas/Message/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

class StubServer : public Atlas::Message::DecoderBase
{ 
public:
    StubServer(short port);
    ~StubServer();

    void run();	// keep the server alive

    typedef enum {
	LISTEN,
	NEGOTIATE,
	FAILURE,
	CONNECTED
    } State;

    // critical override from DecoderBase
    virtual void ObjectArrived(const Atlas::Message::Object& obj);
    
    // the test interface
    bool get(Atlas::Message::Object &obj);
    void push(const Atlas::Message::Object &obj);
    
    void waitForMessage(int timeout);
    
    void setNegotiation(bool enable) {m_doNegotiate=enable;}
    void disconnect();
    
protected:
    State m_state;
    std::queue<Atlas::Message::Object> m_queue;	// all the atlas messages we've received

    void accept();
    void negotiate();

    void fail();
    bool isStreamPending(basic_socket_stream *stream);

    tcp_socket_server* m_listenSocket;
    basic_socket_stream* m_stream;

    // Atlas stuff
    Atlas::Message::Encoder *m_msgEncoder;
    Atlas::Codec<std::iostream>* m_codec;   // Atlas codec.
    Atlas::Net::StreamAccept*m_acceptor;
    
    bool m_doNegotiate;
};

#endif