#ifndef ERIS_TEST_AGENT_H
#define ERIS_TEST_AGENT_CPP

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>
#include <set>
#include <map>

class StubServer;
class ClientConnection;

class Agent
{
public:
    Agent(ClientConnection* con, const std::string& charId);
    ~Agent();
    
    void processOp(const Atlas::Objects::Operation::RootOperation& op);

    void setEntityVisible(const std::string& eid, bool vis);

    static void setEntityVisibleForFutureAgent(const std::string& eid, const std::string& agentId);

private:
    void processLook(const Atlas::Objects::Operation::Look& op);

    bool isVisible(const std::string& eid) const;

    const std::string m_character;
    ClientConnection* m_con;
    StubServer* m_server;
    
    typedef std::set<std::string> EntityIdSet;
    
    EntityIdSet m_visible;
    
    typedef std::multimap<std::string, std::string> StringStringMmap;
    static StringStringMmap static_futureVisible;
};

#endif
