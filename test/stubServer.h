#ifndef ERIS_STUB_SERVER_H
#define ERIS_STUB_SERVER_H

#include <map>
#include <vector>
#include <set>

#include <skstream/skserver.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Entity.h>

class ClientConnection;

typedef std::set<std::string> StringSet;

class StubServer
{ 
public:
    StubServer(short port);
    ~StubServer();

    void run();	// keep the server alive
  
    void setupTestAccounts();

    ClientConnection* getConnectionForAccount(const std::string& accId);
private:
    void joinRoom(const std::string& acc, const std::string& room);
    void partRoom(const std::string& acc, const std::string& room);
    void talkInRoom(const Atlas::Objects::Operation::Talk& tk, const std::string& room);
    
    StringSet peopleInRoom(const std::string& room);
    
    tcp_socket_server m_serverSocket;

    friend class ClientConnection;
    
    std::vector<ClientConnection*> m_clients;
    
    typedef std::map<std::string, Atlas::Objects::Entity::Account> AccountMap;
    AccountMap m_accounts;

    typedef std::map<std::string, Atlas::Objects::Entity::GameEntity> EntityMap;
    EntityMap m_world;
    
    typedef std::map<std::string, Atlas::Objects::Entity::RootEntity> RoomMap;
    RoomMap m_rooms;
};

#endif
