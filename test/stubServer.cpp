#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubServer.h"
#include "clientConnection.h"
#include <Eris/Exceptions.h>

StubServer::StubServer(short port) :
    m_serverSocket(port),
    m_stream(NULL),
    m_acceptor(NULL)
{
    if (!m_serverSocket.is_open())
        throw InvalidOperation("unable to open listen socket");

    // set the linger of the socket to zero, for debugging ease
    linger listenerLinger;
    listenerLinger.l_onoff = 1;
    listenerLinger.l_linger = 0;
    ::setsockopt(m_serverSocket.getSocket(),
        SOL_SOCKET,SO_LINGER,
        (linger*)&listenerLinger,sizeof(listenerLinger));
    
    const int reuseFlag = 1;
    ::setsockopt(m_serverSocket.getSocket(), SOL_SOCKET, SO_REUSEADDR, 
            &reuseFlag, sizeof(reuseFlag));

    debug() << "stub server listening";
}

StubServer::~StubServer()
{
    delete m_acceptor;
    delete m_encoder;
}

void StubServer::run()
{
    if (m_serverSocket.can_accept())
    {
        debug() << "stub server accepting connection";
        m_clients.push_back(new ClientConnect(m_serverSocket.accept()));
    }
    
    for (unsigned int C=0; C < m_clients.size(); ++C)
        m_clients[C]->poll();
}




void StubServer::disconnect()
{
    ERIS_ASSERT(m_state == CONNECTED);
    
    m_stream->close();	// all go bye-bye!
    delete m_stream;
    m_stream = NULL;
    
    m_state = LISTEN;
}

#pragma mark -

void StubServer::sendInfoForType(const std::string &type, const Operation::RootOperation &get)
{
	Operation::Info info;
	Element::ListType &args(info.getArgs());
	
	if (type == "root")
		args.push_back(Atlas::Objects::Root().asObject());
	else if (type == "root_entity") {
		args.push_back(Entity::RootEntity().asObject());
	} else if (type == "game_entity") {
		args.push_back(Entity::GameEntity().asObject());
	} else if (type == "root_operation") {
		args.push_back(Operation::RootOperation().asObject());
	} else if (type == "info") {
		args.push_back(Operation::Info().asObject());
	} else if (type == "get") {
		args.push_back(Operation::Get().asObject());
	} else if (type == "set") {
		args.push_back(Operation::Set().asObject());
	} else if (type == "error") {
		args.push_back(Operation::Error().asObject());
	} else if (type == "create") {
		args.push_back(Operation::Create().asObject());
	} else if (type == "move") {
		args.push_back(Operation::Move().asObject());
	} else if (type == "appearance") {
		args.push_back(Operation::Appearance().asObject());
	} else {
		ERIS_MESSAGE("unknown type in sendInfoForType, responing with ERROR instead");
	
		Operation::Error error;
	
		Element::ListType& eargs(error.getArgs());
		eargs.push_back("undefined type " + type);
		eargs.push_back(get.asObject());
	
		error.setRefno(get.getSerialno());
	
		push(error);
	}
}


void StubServer::join(const std::string& acc, const std::string& room)
{
    assert(m_accounts.count(acc));
    assert(m_rooms.count(room));
    
    if (rooom->contains(acc))
        throw InvalidOperation("duplicate join of room " + room + " by " + acc);
        
    Appearance app;
    
    for ( .. room members ...)
    {
    
    }
}