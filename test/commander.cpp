#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "commander.h"
#include "stubServer.h"

#include <Atlas/Objects/Operation.h>
#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>
#include <Atlas/Objects/Encoder.h>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;




#pragma mark -

Commander::Commander(StubServer* stub, int fd) :
    m_server(stub),
    m_channel(fd)
{


}

Commander::~Commander()
{


}

void Commander::recv()
{
    if (m_acceptor)
        negotiate();
    else {
        m_codec->poll();
        
        while (!m_objDeque.empty())
        {
            RootOperation op = smart_dynamic_cast<RootOperation>(m_objDeque.front());
            if (!op.isValid())
                throw InvalidOperation("ClientConnection recived something that isn't an op");
            
            //
            
            m_objDeque.pop_front();
        }
    }
}

void Commander::objectArrived(const Root& obj)
{
    m_objDeque.push_back(obj);
}

void Commander::negotiate()
{
    m_acceptor->poll(); 
    
    switch (m_acceptor->getState()) {
    case Atlas::Net::StreamAccept::IN_PROGRESS:
        break;

    case Atlas::Net::StreamAccept::FAILED:
        error() << "Commander got Atlas negotiation failure";
        m_channel.close();
        break;

    case Atlas::Net::StreamAccept::SUCCEEDED:
        m_codec = m_acceptor->getCodec();
        m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);
        m_codec->streamBegin();
                
        delete m_acceptor;
        m_acceptor = NULL;
        break;

    default:
        throw InvalidOperation("unknown state from Atlas StreamAccept in Commander::negotiate");
    }             
}

#pragma mark -

void Commander::dispatch(const RootOperation& op)
{
    // stub server control commands, TO = "stub_server"
    // shutdown command
    
    // admin / world modify commands, apply to world state
}
