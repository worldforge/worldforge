#ifndef ERIS_STUB_SERVER_H
#define ERIS_STUB_SERVER_H

#include <map>
#include <vector>
#include <set>
#include <memory>

#include <skstream/skserver.h>

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Entity.h>

class Commander;
class ClientConnection;

typedef std::set<std::string> StringSet;

typedef std::map<std::string, Atlas::Objects::Entity::Account> AccountMap;

class StubServer
{ 
public:
    StubServer(short port, int commandSocket);
    ~StubServer();

    int run(pid_t childPid);	// run the server until the child exists, return a process result
  
    void setupTestAccounts();

    ClientConnection* getConnectionForAccount(const std::string& accId);
    
    AccountMap::const_iterator findAccountByUsername(const std::string &uname);
private:
    void joinRoom(const std::string& acc, const std::string& room);
    void partRoom(const std::string& acc, const std::string& room);
    void talkInRoom(const Atlas::Objects::Operation::Talk& tk, const std::string& room);
    
    StringSet peopleInRoom(const std::string& room);
    
    void subclassType(const std::string& base, const std::string& derivedName);
    
    tcp_socket_server m_serverSocket;
    
    friend class ClientConnection;
    friend class Agent;
    
    typedef std::vector<ClientConnection*> ConArray;
    ConArray m_clients;
    
    AccountMap m_accounts;
    
    typedef std::map<std::string, Atlas::Objects::Entity::GameEntity> EntityMap;
    EntityMap m_world;
    
    typedef std::map<std::string, Atlas::Objects::Entity::RootEntity> RoomMap;
    RoomMap m_rooms;
    
    typedef std::map<std::string, Atlas::Objects::Root> AtlasTypeMap;
    AtlasTypeMap m_types;
    
    std::auto_ptr<Commander> m_command;
};

#endif
