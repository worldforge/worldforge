#ifndef ERIS_TEST_AGENT_H
#define ERIS_TEST_AGENT_CPP

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>

class StubServer;
class ClientConnection;

class Agent
{
public:
    Agent(ClientConnection* con, const std::string& charId);
    ~Agent();
    
    void processOp(const Atlas::Objects::Operation::RootOperation& op);

private:
    void processLook(const Atlas::Objects::Operation::Look& op);

    const std::string m_character;
    ClientConnection* m_con;
    StubServer* m_server;
};

#endif
