#ifndef TEST_COMMANDER_H
#define TEST_COMMANDER_H

#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>

#include <deque>

class StubServer;

class Commander : public Atlas::Objects::ObjectsDecoder
{
public:
    Commander(StubServer* stub, int fd);
    ~Commander();
    
    basic_socket* getStream()
    { return &m_channel; }
    
    void recv();

protected:
    virtual void objectArrived(const Atlas::Objects::Root& obj);
    
private:
    void negotiate();
    void dispatch(const Atlas::Objects::Operation::RootOperation& op);
    
    StubServer* m_server;
    tcp_socket_stream m_channel;
    
    Atlas::Codec* m_codec;
    Atlas::Net::StreamAccept* m_acceptor;
    Atlas::Objects::ObjectsEncoder* m_encoder;
    
    typedef std::deque<Atlas::Objects::Root> RootDeque;
    RootDeque m_objDeque;
};

#endif
