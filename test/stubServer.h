#ifndef ERIS_STUB_SERVER_H
#define ERIS_STUB_SERVER_H

#include <map>
#include <vector>

#include <skstream/skserver.h>
#include <Atlas/Objects/Entity.h>

class StubServer
{ 
public:
    StubServer(short port);
    ~StubServer();

    void run();	// keep the server alive
  
     

private:
    tcp_socket_server m_serverSocket;

    class ClientConnection;
    
    typedef std::vector<ClientConnection*> m_clients;
    
    typedef std::map<std::string, Atlas::Objects::Entity::Account> AccountMap;
    AccountMap m_accounts;

    typedef std::map<std::string, Atlas::Objects::Entty::GameEntity> EntityMap;
    EntityMap m_world;
};

#endif
