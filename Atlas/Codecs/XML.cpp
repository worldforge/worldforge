// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "../Codec.h"
#include "Utility.h"

using namespace std;
using namespace Atlas;

class XML : public Codec<iostream>
{
    public:

    XML(const Codec<iostream>::Parameters&);

    virtual void Poll();

    virtual void StreamEnd() { }

    virtual void MessageBegin();
    virtual void MessageItem(const Map&);
    virtual void MessageEnd();
    
    virtual void MapItem(const std::string& name, const Map&);
    virtual void MapItem(const std::string& name, const List&);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, double);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapEnd();
    
    virtual void ListItem(const Map&);
    virtual void ListItem(const List&);
    virtual void ListItem(int);
    virtual void ListItem(double);
    virtual void ListItem(const std::string&);
    virtual void ListEnd();

    protected:

    iostream& socket;
    Bridge* bridge;
};

namespace
{
    Codec<iostream>::Factory<XML> factory(
	"XML",					    // name
	Codec<iostream>::Metrics(1, 2)		    // metrics
    );
}
    
XML::XML(const Codec<iostream>::Parameters& p)
    : socket(p.stream), bridge(p.bridge)
{
}

void XML::Poll()
{
    // FIXME add polling
}

void XML::MessageBegin()
{
    socket << "<obj>";
}

void XML::MessageItem(const Map&)
{
}

void XML::MessageEnd()
{
    socket << "</obj>";
}

void XML::MapItem(const std::string& name, const Map&)
{
    socket << "<map name=\"" << name << "\">";
}

void XML::MapItem(const std::string& name, const List&)
{
    socket << "<list name=\"" << name << "\">";
}

void XML::MapItem(const std::string& name, int data)
{
    socket << "<int name=\"" << name << "\">" << data << "</int>";
}

void XML::MapItem(const std::string& name, double data)
{
    socket << "<float name=\"" << name << "\">" << data << "</float>";
}

void XML::MapItem(const std::string& name, const std::string& data)
{
    socket << "<string name=\"" << name << "\">" << data << "</string>";
}

void XML::MapEnd()
{
    socket << "</map>";
}

void XML::ListItem(const Map&)
{
    socket << "<map>";
}

void XML::ListItem(const List&)
{
    socket << "<list>";
}

void XML::ListItem(int data)
{
    socket << "<int>" << data << "</int>";
}

void XML::ListItem(double data)
{
    socket << "<float>" << data << "</float>";
}

void XML::ListItem(const std::string& data)
{
    socket << "<string>" << data << "</string>";
}

void XML::ListEnd()
{
    socket << "</list>";
}

