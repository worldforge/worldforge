// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "../Stream/Codec.h"

#include <strstream>

using namespace Atlas::Stream;

class XML : public Codec
{
    public:

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
};

namespace
{
    Codec::Factory<XML> factory("XML", "1.0");
}

void XML::MessageBegin()
{
}

void XML::MessageEnd()
{
}

void XML::ListBegin(const std::string& name)
{
}

void XML::MapBegin(const std::string& name)
{
}

void XML::Item(const std::string& name, int)
{
}

void XML::Item(const std::string& name, float)
{
}

void XML::Item(const std::string& name, const std::string&)
{
}

void XML::Item(const std::string& name, const Atlas::Object&)
{
}

void XML::ListEnd()
{
}

void XML::ListBegin()
{
}

void XML::MapBegin()
{
}

void XML::Item(int)
{
}

void XML::Item(float)
{
}

void XML::Item(const std::string&)
{
}

void XML::Item(const Atlas::Object&)
{
}

void XML::MapEnd()
{
}

/*
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
*/
