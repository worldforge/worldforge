// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Root.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { 

int RootData::getAttrClass(const std::string& name) const
{
    if (name == "id") return ROOT_NO;
    if (name == "parents") return ROOT_NO;
    if (name == "stamp") return ROOT_NO;
    if (name == "objtype") return ROOT_NO;
    if (name == "name") return ROOT_NO;
    return BaseObjectData::getAttrClass(name);
}

const Element RootData::getAttr(const std::string& name) const
    throw (NoSuchAttrException)
{
    if (name == "id") return getId();
    if (name == "parents") return getParentsAsList();
    if (name == "stamp") return getStamp();
    if (name == "objtype") return getObjtype();
    if (name == "name") return getName();
    return BaseObjectData::getAttr(name);
}

int RootData::getAttr(const std::string& name, Element & attr) const
{
    if (name == "id") { attr = getId(); return 0; }
    if (name == "parents") { attr = getParentsAsList(); return 0; }
    if (name == "stamp") { attr = getStamp(); return 0; }
    if (name == "objtype") { attr = getObjtype(); return 0; }
    if (name == "name") { attr = getName(); return 0; }
    return BaseObjectData::getAttr(name, attr);
}

void RootData::setAttr(const std::string& name, const Element& attr)
{
    if (name == "id") { setId(attr.asString()); return; }
    if (name == "parents") { setParentsAsList(attr.asList()); return; }
    if (name == "stamp") { setStamp(attr.asFloat()); return; }
    if (name == "objtype") { setObjtype(attr.asString()); return; }
    if (name == "name") { setName(attr.asString()); return; }
    BaseObjectData::setAttr(name, attr);
}

void RootData::removeAttr(const std::string& name)
{
    if (name == "id")
        { m_attrFlags &= ~ID_FLAG; return;}
    if (name == "parents")
        { m_attrFlags &= ~PARENTS_FLAG; return;}
    if (name == "stamp")
        { m_attrFlags &= ~STAMP_FLAG; return;}
    if (name == "objtype")
        { m_attrFlags &= ~OBJTYPE_FLAG; return;}
    if (name == "name")
        { m_attrFlags &= ~NAME_FLAG; return;}
    BaseObjectData::removeAttr(name);
}

inline void RootData::sendId(Atlas::Bridge & b) const
{
    if(m_attrFlags & ID_FLAG) {
        b.mapStringItem("id", attr_id);
    }
}

inline void RootData::sendParents(Atlas::Bridge & b) const
{
    if((m_attrFlags & PARENTS_FLAG) || !((RootData *)m_defaults)->attr_parents.empty()) {
        b.mapListItem("parents");
        const std::list<std::string> & l = getParents();
        std::list<std::string>::const_iterator I = l.begin();
        for(; I != l.end(); ++I) {
            b.listStringItem(*I);
        }
        b.listEnd();
    }
}

inline void RootData::sendStamp(Atlas::Bridge & b) const
{
    if(m_attrFlags & STAMP_FLAG) {
        b.mapFloatItem("stamp", attr_stamp);
    }
}

inline void RootData::sendObjtype(Atlas::Bridge & b) const
{
    if((m_attrFlags & OBJTYPE_FLAG) || !((RootData *)m_defaults)->attr_objtype.empty()) {
        b.mapStringItem("objtype", getObjtype());
    }
}

inline void RootData::sendName(Atlas::Bridge & b) const
{
    if(m_attrFlags & NAME_FLAG) {
        b.mapStringItem("name", attr_name);
    }
}

void RootData::sendContents(Bridge & b) const
{
    sendId(b);
    sendParents(b);
    sendStamp(b);
    sendObjtype(b);
    sendName(b);
    BaseObjectData::sendContents(b);
}

void RootData::addToMessage(MapType & m) const
{
    BaseObjectData::addToMessage(m);
    if(m_attrFlags & ID_FLAG)
        m["id"] = attr_id;
    const Atlas::Message::ListType l_attr_parents = getParentsAsList();
    if (!l_attr_parents.empty())
        m["parents"] = l_attr_parents;
    if(m_attrFlags & STAMP_FLAG)
        m["stamp"] = attr_stamp;
    const std::string& l_attr_objtype = getObjtype();
    if (!l_attr_objtype.empty())
        m["objtype"] = l_attr_objtype;
    if(m_attrFlags & NAME_FLAG)
        m["name"] = attr_name;
    return;
}

void RootData::iterate(int& current_class, std::string& attr) const
{
    // If we've already finished this class, chain to the parent
    if(current_class >= 0 && current_class != ROOT_NO) {
        BaseObjectData::iterate(current_class, attr);
        return;
    }

    static const char *attr_list[] = {"id","parents","stamp","objtype","name",};
    static const unsigned n_attr = sizeof(attr_list) / sizeof(const char*);

    unsigned next_attr = n_attr; // so we chain to the parent if we don't find attr

    if(attr.empty()) // just staring on this class
        next_attr = 0;
    else {
      for(unsigned i = 0; i < n_attr; ++i) {
         if(attr == attr_list[i]) {
             next_attr = i + 1;
             break;
         }
      }
    }

    if(next_attr == n_attr) { // last one on the list
        current_class = -1;
        attr = "";
        BaseObjectData::iterate(current_class, attr); // chain to parent
    }
    else {
        current_class = ROOT_NO;
        attr = attr_list[next_attr];
    }
}

RootData::~RootData()
{
}

RootData * RootData::copy() const
{
    RootData * copied = RootData::alloc();
    *copied = *this;
    return copied;
}

bool RootData::instanceOf(int classNo) const
{
    if(ROOT_NO == classNo) return true;
    return BaseObjectData::instanceOf(classNo);
}

//freelist related methods specific to this class
RootData *RootData::defaults_RootData = 0;
RootData *RootData::begin_RootData = 0;

RootData *RootData::alloc()
{
    if(begin_RootData) {
        RootData *res = begin_RootData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_RootData = (RootData *)begin_RootData->m_next;
        return res;
    }
    return new RootData(RootData::getDefaultObjectInstance());
}

void RootData::free()
{
    m_next = begin_RootData;
    begin_RootData = this;
}


RootData *RootData::getDefaultObjectInstance()
{
    if (defaults_RootData == 0) {
        defaults_RootData = new RootData;
        defaults_RootData->attr_stamp = 0.0;
        defaults_RootData->attr_objtype = "obj";
        defaults_RootData->attr_parents = std::list<std::string>(1, "root");
    }
    return defaults_RootData;
}

RootData *RootData::getDefaultObject()
{
    return RootData::getDefaultObjectInstance();
}

} } // namespace Atlas::Objects
