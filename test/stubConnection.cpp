#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubConnection.h"

StubConnection::StubConnection() :
    Eris::Connection("stub_con", false)
{
    // make sure it always looks CONNECTED
    _status = Eris::BaseConnection::CONNECTED;
}

StubConnection::~StubConnection()
{
    // stop the base destructor from interfering
    _status = Eris::BaseConnection::DISCONNECTED;
}

void StubConnection::clear()
{
    m_queue.clear();
}

void StubConnection::push(const Atlas::Message::Element &obj)
{
    objectArrived(obj);
}

void StubConnection::send(const Atlas::Message::Element &obj)
{
    m_queue.push_back(obj);
}

void StubConnection::send(const Atlas::Objects::Root &obj)
{
    send(obj.asObject());
}

bool StubConnection::get(Atlas::Message::Element &obj)
{
    if (m_queue.empty()) return false;
    obj = m_queue.front();
    m_queue.pop_front();
    return true;
}
