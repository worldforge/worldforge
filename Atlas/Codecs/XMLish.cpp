// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Utility.h"
#include "../Codec.h"

using namespace std;
using namespace Atlas;

/*

    FIXME documentation forthcoming

*/

class XMLish : public Codec<iostream>
{
    public:

    XMLish(const Codec<iostream>::Parameters&);

    virtual void Poll();

    virtual void StreamBegin();
    virtual void StreamEnd();

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
    Codec<iostream>::Factory<XMLish> factory(
	"XMLish",				    // name
	Codec<iostream>::Metrics(1, 2)		    // metrics
    );
}
    
XMLish::XMLish(const Codec<iostream>::Parameters& p)
    : socket(p.stream), bridge(p.bridge)
{
}

void XMLish::Poll()
{
    while (socket.rdbuf()->in_avail()/* || socket.rdbuf()->showmanyc() */)
    {
	char next = socket.get();

	// FIXME handle incoming characters
    }
}

void XMLish::StreamBegin()
{
    socket << "<atlas>";
}

void XMLish::StreamEnd()
{
    socket << "</atlas>";
}

void XMLish::MessageBegin()
{
    socket << "<obj>";
}

void XMLish::MessageItem(const Map&)
{
}

void XMLish::MessageEnd()
{
    socket << "</obj>";
}

void XMLish::MapItem(const std::string& name, const Map&)
{
    socket << "<map name=\"" << name << "\">";
}

void XMLish::MapItem(const std::string& name, const List&)
{
    socket << "<list name=\"" << name << "\">";
}

void XMLish::MapItem(const std::string& name, int data)
{
    socket << "<int name=\"" << name << "\">" << data << "</int>";
}

void XMLish::MapItem(const std::string& name, double data)
{
    socket << "<float name=\"" << name << "\">" << data << "</float>";
}

void XMLish::MapItem(const std::string& name, const std::string& data)
{
    socket << "<string name=\"" << name << "\">" << data << "</string>";
}

void XMLish::MapEnd()
{
    socket << "</map>";
}

void XMLish::ListItem(const Map&)
{
    socket << "<map>";
}

void XMLish::ListItem(const List&)
{
    socket << "<list>";
}

void XMLish::ListItem(int data)
{
    socket << "<int>" << data << "</int>";
}

void XMLish::ListItem(double data)
{
    socket << "<float>" << data << "</float>";
}

void XMLish::ListItem(const std::string& data)
{
    socket << "<string>" << data << "</string>";
}

void XMLish::ListEnd()
{
    socket << "</list>";
}

