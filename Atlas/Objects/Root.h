// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ROOT_H
#define ATLAS_OBJECTS_ROOT_H

#include <string>
#include <map>
#include <utility>
#include "../Bridge.h"
#include "../Message/Object.h"
#include "Macros.h"

namespace Atlas { namespace Objects {

class Root
{
public:
    Root();
    
    virtual ~Root();
    
    virtual void Clear();
    virtual void Reset();
    
    virtual Atlas::Message::Object GetAttr(const std::string&) const;
    virtual void SetAttr(const std::string&, const Atlas::Message::Object&);
    virtual bool HasAttr(const std::string&) const;
    
    virtual void Transmit(Atlas::Bridge* b);

    METHODS(std::string, id)

protected:
    
    typedef std::map< std::string,
              std::pair<bool, Atlas::Message::Object> > attrmap;

    attrmap attributes;

    ATTRIBUTE(std::string, id)
};

} }

#endif
