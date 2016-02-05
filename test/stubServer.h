#ifndef ERIS_STUB_SERVER_H
#define ERIS_STUB_SERVER_H

#include <map>
#include <vector>
#include <set>
#include <memory>

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Entity.h>

class Commander;
class ClientConnection;
class Agent;

typedef std::set<std::string> StringSet;

typedef std::map<std::string, Atlas::Objects::Entity::Account> AccountMap;

class StubServer
{ 
public:
    StubServer(short port);
    ~StubServer();

    int run(pid_t childPid);	// run the server until the child exists, return a process result
  
    void setupTestAccounts();

    ClientConnection* getConnectionForAccount(const std::string& accId);
    
    AccountMap::const_iterator findAccountByUsername(const std::string &uname);
    
    Agent* findAgentForEntity(const std::string& eid);
    
    Atlas::Objects::Entity::RootEntity getEntity(const std::string& eid) const;
    
    void resetWorld();
    void addManyObjects(const std::string& );
private:
    void joinRoom(const std::string& acc, const std::string& room);
    void partRoom(const std::string& acc, const std::string& room);
    void talkInRoom(const Atlas::Objects::Operation::Talk& tk, const std::string& room);
    
    StringSet peopleInRoom(const std::string& room);
    
    void subclassType(const std::string& base, const std::string& derivedName);
    
    void defineEntity(const std::string& id, 
        const std::string& type,
        const std::string& loc,
        const std::string& nm);
    
    void initCalendarOn(Atlas::Objects::Entity::RootEntity& ent);
    
    tcp_socket_server m_serverSocket;
    
    friend class ClientConnection;
    friend class Agent;
    friend class Commander;
    
    typedef std::vector<ClientConnection*> ConArray;
    ConArray m_clients;
    
    AccountMap m_accounts;
    
    typedef std::map<std::string, Atlas::Objects::Entity::RootEntity> EntityMap;
    EntityMap m_world;
    
    typedef std::map<std::string, Atlas::Objects::Entity::RootEntity> RoomMap;
    RoomMap m_rooms;
    
    typedef std::map<std::string, Atlas::Objects::Root> AtlasTypeMap;
    AtlasTypeMap m_types;
    
    unix_socket_server m_commandSocket;
    std::unique_ptr<Commander> m_command;
};

#endif
