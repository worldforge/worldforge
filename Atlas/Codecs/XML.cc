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

    virtual void Initialise(Socket*, Filter*);

    virtual void MessageBegin();
    virtual void MessageEnd();
    
    virtual void ListBegin(const std::string& name);
    virtual void MapBegin(const std::string& name);
    virtual void Item(const std::string& name, int);
    virtual void Item(const std::string& name, float);
    virtual void Item(const std::string& name, const std::string&);
    virtual void Item(const std::string& name, const Atlas::Object&);
    virtual void ListEnd();
    
    virtual void ListBegin();
    virtual void MapBegin();
    virtual void Item(int);
    virtual void Item(float);
    virtual void Item(const std::string&);
    virtual void Item(const Atlas::Object&);
    virtual void MapEnd();

    protected:

    Socket* socket;
    Filter* filter;
};

namespace
{
    Codec::Factory<XML> factory("XML", Codec::Metrics(1, 2));
}
    
void XML::Initialise(Socket* s, Filter* f)
{
    socket = s;
    filter = f;
}

void XML::MessageBegin()
{
    socket->Send("<obj>");
}

void XML::MessageEnd()
{
    socket->Send("</obj>");
}

void XML::ListBegin(const std::string& name)
{
    socket->Send("<list name=\"");
    socket->Send(name);
    socket->Send("\">");
}

void XML::MapBegin(const std::string& name)
{
    socket->Send("<map name=\"");
    socket->Send(name);
    socket->Send("\">");
}

void XML::Item(const std::string& name, int data)
{
    socket->Send("<int name=\"");
    socket->Send(name);
    socket->Send("\">");
    // FIXME send the int here
    socket->Send("</int>");
}

void XML::Item(const std::string& name, float data)
{
    socket->Send("<float name=\"");
    socket->Send(name);
    socket->Send("\">");
    // FIXME send the float here
    socket->Send("</float>");
}

void XML::Item(const std::string& name, const std::string& data)
{
    socket->Send("<string name=\"");
    socket->Send(name);
    socket->Send("\">");
    socket->Send(data);
    socket->Send("</string>");
}

void XML::Item(const std::string& name, const Atlas::Object& data)
{
    // FIXME recurse through object and send it
}

void XML::ListEnd()
{
    socket->Send("</list>");
}

void XML::ListBegin()
{
    socket->Send("<list>");
}

void XML::MapBegin()
{
    socket->Send("<map>");
}

void XML::Item(int data)
{
    socket->Send("<int>");
    // FIXME send data
    socket->Send("</int>");
}

void XML::Item(float data)
{
    socket->Send("<float>");
    // FIXME send data
    socket->Send("</float>");
}

void XML::Item(const std::string& data)
{
    socket->Send("<string>");
    socket->Send(data);
    socket->Send("</string>");
}

void XML::Item(const Atlas::Object& data)
{
    // FIXME recurse through object and send it
}

void XML::MapEnd()
{
    socket->Send("</map>");
}

