#ifndef ERIS_TEST_CLIENTCON_H
#define ERIS_TEST_CLIENTCON_H

#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>

#include <skstream/skstream.h>
#include <deque>

class StubServer;

class ClientConnection : public Atlas::Objects::ObjectsDecoder
{
public:
    ClientConnection(StubServer* ss, int socket);
    ~ClientConnection();

    void poll();

    bool isDisconnected();

    basic_socket* getStream()
    {
        return &m_stream;
    }

    const std::string& getAccount() const
    { return m_account; }

    //void handleOperation(const Atlas::Objects::Operation::RootOperation& op);

    void send(const Atlas::Objects::Root& obj);

// critical override from ObjectsDecoder
    virtual void objectArrived(const Atlas::Objects::Root& obj);

private:
    void negotiate();
    void fail();

    void dispatch(const Atlas::Objects::Operation::RootOperation& op);
    void dispatchOOG(const Atlas::Objects::Operation::RootOperation& op);

    void processLogin(const Atlas::Objects::Operation::Login& login);
    void processAccountCreate(const Atlas::Objects::Operation::Create& cr);
    void processOOGLook(const Atlas::Objects::Operation::Look& lk);
    void processAnonymousGet(const Atlas::Objects::Operation::Get& get);

    void sendError(const std::string& msg, const Atlas::Objects::Operation::RootOperation& op);

    bool entityIsCharacter(const std::string& id);

    tcp_socket_stream m_stream;
    std::string m_account;
    StubServer* m_server;

    typedef std::deque<Atlas::Objects::Root> RootDeque;
    RootDeque m_objDeque;

// Atlas stuff
    Atlas::Codec* m_codec;
    Atlas::Net::StreamAccept* m_acceptor;
    Atlas::Objects::ObjectsEncoder* m_encoder;
};

#endif
