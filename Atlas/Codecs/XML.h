// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.1  2000-02-23 06:51:28  mike
// Please state the nature of your medical emergency.
//

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
    
    virtual void BeginList();
    virtual void BeginList(const std::string& name);
    virtual void EndList();

    virtual void BeginMap();
    virtual void BeginMap(const std::string& name);
    virtual void EndMap();
    
    virtual void Send(int);
    virtual void Send(float);
    virtual void Send(const std::string&);
    virtual void Send(const Atlas::Object&);
	
    virtual void Send(const std::string& name, int);
    virtual void Send(const std::string& name, float);
    virtual void Send(const std::string& name, const std::string&);
    virtual void Send(const std::string& name, const Atlas::Object&);
};

#endif

