#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubConnection.h"

void StubConnection::clear()
{
    m_queue.clear();
}

void StubConnection::push(const Atlas::Message::Object &obj)
{
    ObjectArrived(obj);
}

void StubConnection::send(const Atlas::Message::Object &obj)
{
    m_queue.push_back(obj);
}

void StubConnection::send(const Atlas::Objects::Root &obj)
{
    send(obj.AsObject());
}

bool StubConnection::get(Atlas::Message::Object &obj)
{
    if (m_queue.empty()) return false;
    obj = m_queue.front();
    m_queue.pop_front();
    return true;
}