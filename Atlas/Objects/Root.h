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

namespace Atlas { namespace Objects {

class Root
{
public:
    Root();
    
    virtual ~Root();
    
    virtual void Clear();
    virtual void Reset();
    
    virtual Atlas::Message::Object Get(const std::string&);
    virtual void Set(const std::string&, const Atlas::Message::Object&);
    virtual bool Has(const std::string&);
    
    virtual void Transmit(Atlas::Bridge* b);

    virtual std::string GetId();
    virtual void SetId(const std::string&);
    virtual bool HasId();
    
protected:
    
    typedef std::map< std::string,
              std::pair<bool, Atlas::Message::Object> > attrmap;

    attrmap attributes;
    
    string id;
};

} }

#endif
