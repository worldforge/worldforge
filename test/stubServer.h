#ifndef ERIS_STUB_SERVER_H
#define ERIS_STUB_SERVER_H

#include <skstream/skstream.h>
#include <skstream/skserver.h>

#include <queue>

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

namespace Atlas { namespace Objects {
	class Encoder;
	class Root;
		
	namespace Operation {
		class RootOperation;
	}
	
	namespace Entity {
	
	}
}}

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
    virtual void objectArrived(const Atlas::Message::Element& obj);
    
    // the test interface
    bool get(Atlas::Message::Element &obj);
    void push(const Atlas::Message::Element &obj);
    void push(const Atlas::Objects::Root &obj);
	
    void waitForMessage(int timeout);
    
    void setNegotiation(bool enable) {m_doNegotiate=enable;}
    void disconnect();
    
protected:
    void sendInfoForType(const std::string &type, const Atlas::Objects::Operation::RootOperation &get);

	State m_state;
    std::queue<Atlas::Message::Element> m_queue;	// all the atlas messages we've received

    void accept();
    bool can_accept();

    void negotiate();

    void fail();
    bool isStreamPending(basic_socket_stream *stream);

    SOCKET_TYPE m_listenSocket;
    basic_socket_stream* m_stream;

    // Atlas stuff
    Atlas::Message::Encoder *m_msgEncoder;
    Atlas::Codec<std::iostream>* m_codec;   // Atlas codec.
    Atlas::Net::StreamAccept*m_acceptor;
    Atlas::Objects::Encoder* m_objectEncoder;
	
    bool m_doNegotiate;
	
	/** this flag is set if the stub server should automatically process and respond to
	type queries, without them ever entering the queue. This behaviour can be disabled
	to test the type info logic in eris reliably. */
	bool m_handleTypeQueries;
};

#endif
