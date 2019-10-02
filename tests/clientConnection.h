#ifndef ERIS_TEST_CLIENTCON_H
#define ERIS_TEST_CLIENTCON_H

#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>

#include <deque>

class StubServer;
class Agent;

class ClientConnection : public Atlas::Objects::ObjectsDecoder
{
public:
    ClientConnection(StubServer* ss, int socket);
    ~ClientConnection();

    void poll();

    bool isDisconnected();

    basic_socket* getStream()
    { return &m_stream; }

    StubServer* getServer()
    { return m_server; }

    const std::string& getAccount() const
    { return m_account; }

    Agent* findAgentForEntity(const std::string& eid) const;

    //void handleOperation(const Atlas::Objects::Operation::RootOperation& op);

    void send(const Atlas::Objects::Root& obj);

// critical override from ObjectsDecoder
    virtual void objectArrived(const Atlas::Objects::Root& obj);

    void sendError(const std::string& msg, const Atlas::Objects::Operation::RootOperation& op);
    
    void shutdown();
private:
    void negotiate();
    void fail();

    void dispatch(const Atlas::Objects::Operation::RootOperation& op);
    void dispatchOOG(const Atlas::Objects::Operation::RootOperation& op);

    void processLogin(const Atlas::Objects::Operation::Login& login);
    void processAccountCreate(const Atlas::Objects::Operation::Create& cr);
    void processOOGLook(const Atlas::Objects::Operation::Look& lk);
    void processAnonymousGet(const Atlas::Objects::Operation::Get& get);
    void processTalk(const Atlas::Objects::Operation::Talk& tk);
    
    void createCharacter(const Atlas::Objects::Operation::RootOperation& op);
    void activateCharacter(const std::string& charId, const Atlas::Objects::Operation::RootOperation& op);
    
    void processIG(const Atlas::Objects::Operation::RootOperation& op);
    void igLook(const Atlas::Objects::Operation::Look& look);
    
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
    
    typedef std::map<std::string, Agent*> AgentMap;
    AgentMap m_agents;
};

#endif
