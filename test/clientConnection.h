#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>

#include <skstream/skstream.h>
#include <deque>

class Account::ClientConnection : public Atlas::Objects::DecoderBase
{
public:
    ClientConnect(int socket);

    void poll();
    
    //void handleOperation(const Atlas::Objects::Operation::RootOperation& op);
    
    
// critical override from ObjectsDecoder
    virtual void objectArrived(const Atlas::Objects::Root& obj);
    
private:
    void negotiate();
    void fail();
    
    void dispatch(const Atlas::Objects::Operation::RootOperation& op);
    
    basic_socket_stream m_stream;
    std::string m_account;
    
    typedef std::deque<Atlas::Objects::Root> RootDeque;
    RootDeque m_objDeque;
    
// Atlas stuff
    Atlas::Codec* m_codec;
    Atlas::Net::StreamAccept* m_acceptor;
    Atlas::Objects::Encoder* m_encoder;
};