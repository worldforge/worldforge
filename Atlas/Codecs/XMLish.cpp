// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#include <Atlas/Codecs/Utility.h>
#include <Atlas/Codec.h>

#include <iostream>

using namespace Atlas;

/*

    FIXME documentation forthcoming

*/

class XMLish : public Codec<std::iostream>
{
    public:

    XMLish(const Codec<std::iostream>::Parameters&);

    virtual void poll();

    virtual void streamBegin();
    virtual void streamMessage();
    virtual void streamEnd();

    virtual void mapItem(const std::string& name, const Map&);
    virtual void mapItem(const std::string& name, const List&);
    virtual void mapItem(const std::string& name, long);
    virtual void mapItem(const std::string& name, double);
    virtual void mapItem(const std::string& name, const std::string&);
    virtual void mapEnd();
    
    virtual void listItem(const Map&);
    virtual void listItem(const List&);
    virtual void listItem(long);
    virtual void listItem(double);
    virtual void listItem(const std::string&);
    virtual void listEnd();

    protected:

    std::iostream& socket;
    Bridge* bridge;
};

namespace
{
    /*
        We're not going to enable this sucker until it actually works.
    
    Codec<std::iostream>::Factory<XMLish> factory(
	"XMLish",				    // name
	Codec<std::iostream>::Metrics(1, 2)		    // metrics
    );

    */
}
    
XMLish::XMLish(const Codec<std::iostream>::Parameters& p)
    : socket(p.stream), bridge(p.bridge)
{
}

void XMLish::poll()
{
    if (!can_read) return;
    do
    {
	char next = socket.get();

	// FIXME handle incoming characters
    }
    while (socket.rdbuf()->in_avail());
}

void XMLish::streamBegin()
{
    socket << "<atlas>";
}

void XMLish::streamMessage()
{
    socket << "<map>";
}

void XMLish::streamEnd()
{
    socket << "</atlas>";
}

void XMLish::mapItem(const std::string& name, const Map&)
{
    socket << "<map name=\"" << name << "\">";
}

void XMLish::mapItem(const std::string& name, const List&)
{
    socket << "<list name=\"" << name << "\">";
}

void XMLish::mapItem(const std::string& name, long data)
{
    socket << "<int name=\"" << name << "\">" << data << "</int>";
}

void XMLish::mapItem(const std::string& name, double data)
{
    socket << "<float name=\"" << name << "\">" << data << "</float>";
}

void XMLish::mapItem(const std::string& name, const std::string& data)
{
    socket << "<string name=\"" << name << "\">" << data << "</string>";
}

void XMLish::mapEnd()
{
    socket << "</map>";
}

void XMLish::listItem(const Map&)
{
    socket << "<map>";
}

void XMLish::listItem(const List&)
{
    socket << "<list>";
}

void XMLish::listItem(long data)
{
    socket << "<int>" << data << "</int>";
}

void XMLish::listItem(double data)
{
    socket << "<float>" << data << "</float>";
}

void XMLish::listItem(const std::string& data)
{
    socket << "<string>" << data << "</string>";
}

void XMLish::listEnd()
{
    socket << "</list>";
}

