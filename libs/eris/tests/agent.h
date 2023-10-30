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

    static void broadcastSight(const Atlas::Objects::Operation::RootOperation& op);
    static void broadcastSound(const Atlas::Objects::Operation::RootOperation& op);
    
private:
    void processLook(const Atlas::Objects::Operation::Look& op);
    void processWield(const Atlas::Objects::Operation::RootOperation& op);

    bool isVisible(const std::string& eid) const;

    std::string randomVisibleEntity() const;
    std::string randomInvisibleEntity() const;
    
    const std::string m_character;
    ClientConnection* m_con;
    StubServer* m_server;
    
    typedef std::set<std::string> EntityIdSet;
    
    EntityIdSet m_visible;
    
    typedef std::multimap<std::string, std::string> StringStringMmap;
    static StringStringMmap static_futureVisible;
    
    typedef std::set<Agent*> AgentSet;
    static AgentSet static_allAgents;
};

#endif
