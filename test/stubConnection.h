#ifndef STUB_CONNECTION_H
#define STUB_CONNECTION_H

#include <list>
#include <Eris/Connection.h>
 
class StubConnection : public Eris::Connection
{
public:
    StubConnection();
    virtual ~StubConnection();
	
    // over-ride the necessary connection methods to let us do magic

    virtual void send(const Atlas::Message::Object &obj);
    virtual void send(const Atlas::Objects::Root &obj);

    // the test interface
    bool get(Atlas::Message::Object &obj);
    void push(const Atlas::Message::Object &obj);
	
    // reset any previous crap
    void clear();
	
protected:
    std::list<Atlas::Message::Object> m_queue;
};

#endif
