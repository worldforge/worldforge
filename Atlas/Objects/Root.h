// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ROOT_H
#define ATLAS_OBJECTS_ROOT_H

#include <map>
#include <string>
#include "../Message/Object.h"
#include "../Bridge.h"

namespace Atlas { namespace Objects {

class NoSuchAttrException
{
public:
    NoSuchAttrException(const std::string& name) : name(name) {}
    std::string name;
};
    
class Root {
public:
    Root();
    Root(const string& id);
    virtual ~Root();

    static Root Instantiate();

    virtual bool HasAttr(const std::string& name) const;
    virtual const Atlas::Message::Object& GetAttr(const std::string& name)
        const throw (NoSuchAttrException);
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    virtual void RemoveAttr(const std::string& name);
    
    virtual Atlas::Message::Object AsObject() const;

    virtual void SendContents(Atlas::Bridge* b) const;

protected:
    std::map<std::string, Atlas::Message::Object> attributes;
};

} }

#endif
