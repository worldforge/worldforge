// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_CODECS_XML_H
#define ATLAS_CODECS_XML_H

#include "../Stream/Codec.h"

namespace Atlas
{
    namespace Codecs
    {
	class XML;
    }
}

class Atlas::Codecs::XML : public Atlas::Stream::Codec
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

#endif

