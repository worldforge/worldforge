// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.1  2000-02-23 06:51:28  mike
// Please state the nature of your medical emergency.
//

#include "XML.h"

#include <strstream>
    
void Atlas::Codecs::XML::BeginList()
{
    output("<list>");
}

void Atlas::Codecs::XML::BeginList(const std::string& name)
{
    output("<list name=\"" + name + "\">");
}

void Atlas::Codecs::XML::EndList()
{
    output("</list>");
}

void Atlas::Codecs::XML::BeginMap()
{
    output("<map>");
}

void Atlas::Codecs::XML::BeginMap(const std::string& name)
{
    output("<map name=\"" + name + "\">");
}

void Atlas::Codecs::XML::EndMap()
{
    output("</map>");
}

void Atlas::Codecs::XML::Send(int i)
{
    ostrstream buffer;
    buffer << "<int>" << i << "</int>";
    output(buffer.str());
    buffer.freeze(0);
}

void Atlas::Codecs::XML::Send(float f)
{
    ostrstream buffer;
    buffer << "<float>" << f << "</float>";
    output(buffer.str());
    buffer.freeze(0);
}

void Atlas::Codecs::XML::Send(const std::string& s)
{
    ostrstream buffer;
    buffer << "<string>" << s << "</string>";
    output(buffer.str());
    buffer.freeze(0);
}

void Atlas::Codecs::XML::Send(const Atlas::Object& obj)
{

}

void Atlas::Codecs::XML::Send(const std::string& name, int i)
{
    ostrstream buffer;
    buffer << "<int name=\"" << name << "\">" << i << "</int>";
    output(buffer.str());
    buffer.freeze(0);
}

void Atlas::Codecs::XML::Send(const std::string& name, float f)
{
    ostrstream buffer;
    buffer << "<float name=\"" << name << "\">" << f << "</int>";
    output(buffer.str());
    buffer.freeze(0);
}

void Atlas::Codecs::XML::Send(const std::string& name, const std::string& s)
{
    ostrstream buffer;
    buffer << "<string name=\"" << name << "\">" << s << "</int>";
    output(buffer.str());
    buffer.freeze(0);
}

void Atlas::Codecs::XML::Send(const std::string& name, const Atlas::Object& obj)
{
}
