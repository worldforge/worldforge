// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "../Stream/Codec.h"

#include <strstream>

using namespace std;
using namespace Atlas::Net;
using namespace Atlas::Stream;

class XML : public Codec
{
    public:

    virtual void Initialise(Socket*, Filter*, Bridge*);

    virtual void MessageBegin();
    virtual void MessageMapBegin();
    virtual void MessageEnd();
    
    virtual void MapListBegin(const std::string& name);
    virtual void MapMapBegin(const std::string& name);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, float);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapItem(const std::string& name, const Atlas::Object&);
    virtual void MapEnd();
    
    virtual void ListListBegin();
    virtual void ListMapBegin();
    virtual void ListItem(int);
    virtual void ListItem(float);
    virtual void ListItem(const std::string&);
    virtual void ListItem(const Atlas::Object&);
    virtual void ListEnd();

    protected:

    Socket* socket;
    Filter* filter;
    Bridge* bridge;
};

namespace
{
    Codec::Factory<XML> factory("XML", Codec::Metrics(1, 2));
}
    
void XML::Initialise(Socket* s, Filter* f, Bridge* b)
{
    socket = s;
    filter = f;
    bridge = b;
}

void XML::MessageBegin()
{
    socket->Send("<obj>");
}

void XML::MessageMapBegin()
{
}

void XML::MessageEnd()
{
    socket->Send("</obj>");
}

void XML::MapListBegin(const std::string& name)
{
    socket->Send("<list name=\"");
    socket->Send(name);
    socket->Send("\">");
}

void XML::MapMapBegin(const std::string& name)
{
    socket->Send("<map name=\"");
    socket->Send(name);
    socket->Send("\">");
}

void XML::MapItem(const std::string& name, int data)
{
    socket->Send("<int name=\"");
    socket->Send(name);
    socket->Send("\">");
    // FIXME send the int here
    socket->Send("</int>");
}

void XML::MapItem(const std::string& name, float data)
{
    socket->Send("<float name=\"");
    socket->Send(name);
    socket->Send("\">");
    // FIXME send the float here
    socket->Send("</float>");
}

void XML::MapItem(const std::string& name, const std::string& data)
{
    socket->Send("<string name=\"");
    socket->Send(name);
    socket->Send("\">");
    socket->Send(data);
    socket->Send("</string>");
}

void XML::MapItem(const std::string& name, const Atlas::Object& data)
{
    // FIXME recurse through object and send it
}

void XML::MapEnd()
{
    socket->Send("</map>");
}

void XML::ListListBegin()
{
    socket->Send("<list>");
}

void XML::ListMapBegin()
{
    socket->Send("<map>");
}

void XML::ListItem(int data)
{
    socket->Send("<int>");
    // FIXME send data
    socket->Send("</int>");
}

void XML::ListItem(float data)
{
    socket->Send("<float>");
    // FIXME send data
    socket->Send("</float>");
}

void XML::ListItem(const std::string& data)
{
    socket->Send("<string>");
    socket->Send(data);
    socket->Send("</string>");
}

void XML::ListItem(const Atlas::Object& data)
{
    // FIXME recurse through object and send it
}

void XML::ListEnd()
{
    socket->Send("</list>");
}

