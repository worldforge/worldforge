// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "../Stream/Codec.h"

#include "Utility.h"

using namespace std;
using namespace Atlas::Stream;

/** Packed ASCII codec

[type][name]=[data]
  
{} for message
() for lists
[] for maps
$ for string
@ for int
# for float

*/

class PackedAscii : public Codec
{
public:
    
    PackedAscii(iostream&, Filter*, Bridge*);

    virtual void Initialise(iostream&, Filter*, Bridge*);

    virtual void MessageBegin();
    virtual void MessageMapBegin();
    virtual void MessageEnd();
    
    virtual void MapItem(const std::string& name, const Map&);
    virtual void MapItem(const std::string& name, const List&);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, float);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapItem(const std::string& name, const Atlas::Object&);
    virtual void MapEnd();
    
    virtual void ListItem(const Map&);
    virtual void ListItem(const List&);
    virtual void ListItem(int);
    virtual void ListItem(float);
    virtual void ListItem(const std::string&);
    virtual void ListItem(const Atlas::Object&);
    virtual void ListEnd();

protected:
    
    iostream& socket;
    Filter* filter;
    Bridge* bridge;
};

namespace {
    Codec::Factory<PackedAscii> factor("PackedAscii", Codec::Metrics(1, 2));
}

PackedAscii::PackedAscii(iostream& socket, Filter* f, Bridge* b) :
    socket(socket), filter(f), bridge(b)
{
}

void PackedAscii::MessageBegin()
{
    socket << "{";
}

void PackedAscii::MessageMapBegin()
{
    socket << "[=";
}

void PackedAscii::MessageEnd()
{
    socket << "}";
}

void PackedAscii::MapItem(const std::string& name, const Map&)
{
    socket << "[" << name << "=";
}

void PackedAscii::MapItem(const std::string& name, const List&)
{
    socket << "(" << name << "=";
}

void PackedAscii::MapItem(const std::string& name, int data)
{
    socket << "@" << name << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, float data)
{
    socket << "#" << name << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, const std::string& data)
{
    socket << "$" << name << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, const Atlas::Object& data)
{
    // FIXME recursive...
}

void PackedAscii::MapEnd()
{
    socket << "]";
}

void PackedAscii::ListItem(const Map&)
{
    socket << "[=";
}

void PackedAscii::ListItem(const List&)
{
    socket << "(=";
}

void PackedAscii::ListItem(int data)
{
    socket << "@=" << data;
}

void PackedAscii::ListItem(float data)
{
    socket << "#=" << data;
}

void PackedAscii::ListItem(const std::string& data)
{
    socket << "$=" << data;
}

void PackedAscii::ListItem(const Atlas::Object& data)
{
    // FIXME recursive...
}

void PackedAscii::ListEnd()
{
    socket << ")";
}
