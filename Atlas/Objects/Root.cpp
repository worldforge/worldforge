// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Root.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { 

const std::string ID_ATTR = "id";
const std::string PARENT_ATTR = "parent";
const std::string STAMP_ATTR = "stamp";
const std::string OBJTYPE_ATTR = "objtype";
const std::string NAME_ATTR = "name";

int RootData::getAttrClass(const std::string& name) const
{
    if (allocator.attr_flags_Data.find(name) != allocator.attr_flags_Data.end()) {
        return ROOT_NO;
    }
    return BaseObjectData::getAttrClass(name);
}

int RootData::getAttrFlag(const std::string& name) const
{
    std::map<std::string, int>::const_iterator I = allocator.attr_flags_Data.find(name);
    if (I != allocator.attr_flags_Data.end()) {
        return I->second;
    }
    return BaseObjectData::getAttrFlag(name);
}

int RootData::copyAttr(const std::string& name, Element & attr) const
{
    if (name == ID_ATTR) { attr = getId(); return 0; }
    if (name == PARENT_ATTR) { attr = getParent(); return 0; }
    if (name == STAMP_ATTR) { attr = getStamp(); return 0; }
    if (name == OBJTYPE_ATTR) { attr = getObjtype(); return 0; }
    if (name == NAME_ATTR) { attr = getName(); return 0; }
    return BaseObjectData::copyAttr(name, attr);
}

void RootData::setAttr(const std::string& name, const Element& attr)
{
    if (name == ID_ATTR) { setId(attr.asString()); return; }
    if (name == PARENT_ATTR) { setParent(attr.asString()); return; }
    if (name == STAMP_ATTR) { setStamp(attr.asFloat()); return; }
    if (name == OBJTYPE_ATTR) { setObjtype(attr.asString()); return; }
    if (name == NAME_ATTR) { setName(attr.asString()); return; }
    BaseObjectData::setAttr(name, attr);
}

void RootData::removeAttr(const std::string& name)
{
    if (name == ID_ATTR)
        { m_attrFlags &= ~ID_FLAG; return;}
    if (name == PARENT_ATTR)
        { m_attrFlags &= ~PARENT_FLAG; return;}
    if (name == STAMP_ATTR)
        { m_attrFlags &= ~STAMP_FLAG; return;}
    if (name == OBJTYPE_ATTR)
        { m_attrFlags &= ~OBJTYPE_FLAG; return;}
    if (name == NAME_ATTR)
        { m_attrFlags &= ~NAME_FLAG; return;}
    BaseObjectData::removeAttr(name);
}

inline void RootData::sendId(Atlas::Bridge & b) const
{
    if(m_attrFlags & ID_FLAG) {
        b.mapStringItem(ID_ATTR, attr_id);
    }
}

inline void RootData::sendParent(Atlas::Bridge & b) const
{
    if((m_attrFlags & PARENT_FLAG) || !((RootData *)m_defaults)->attr_parent.empty()) {
        b.mapStringItem(PARENT_ATTR, getParent());
    }
}

inline void RootData::sendStamp(Atlas::Bridge & b) const
{
    if(m_attrFlags & STAMP_FLAG) {
        b.mapFloatItem(STAMP_ATTR, attr_stamp);
    }
}

inline void RootData::sendObjtype(Atlas::Bridge & b) const
{
    if((m_attrFlags & OBJTYPE_FLAG) || !((RootData *)m_defaults)->attr_objtype.empty()) {
        b.mapStringItem(OBJTYPE_ATTR, getObjtype());
    }
}

inline void RootData::sendName(Atlas::Bridge & b) const
{
    if(m_attrFlags & NAME_FLAG) {
        b.mapStringItem(NAME_ATTR, attr_name);
    }
}

void RootData::sendContents(Bridge & b) const
{
    sendId(b);
    sendParent(b);
    sendStamp(b);
    sendObjtype(b);
    sendName(b);
    BaseObjectData::sendContents(b);
}

void RootData::addToMessage(MapType & m) const
{
    BaseObjectData::addToMessage(m);
    if(m_attrFlags & ID_FLAG)
        m[ID_ATTR] = attr_id;
    const std::string& l_attr_parent = getParent();
    if (!l_attr_parent.empty())
        m[PARENT_ATTR] = l_attr_parent;
    if(m_attrFlags & STAMP_FLAG)
        m[STAMP_ATTR] = attr_stamp;
    const std::string& l_attr_objtype = getObjtype();
    if (!l_attr_objtype.empty())
        m[OBJTYPE_ATTR] = l_attr_objtype;
    if(m_attrFlags & NAME_FLAG)
        m[NAME_ATTR] = attr_name;
    return;
}

void RootData::iterate(int& current_class, std::string& attr) const
{
    // If we've already finished this class, chain to the parent
    if(current_class >= 0 && current_class != ROOT_NO) {
        BaseObjectData::iterate(current_class, attr);
        return;
    }

    static const char *attr_list[] = {"id","parent","stamp","objtype","name",};
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

Allocator<RootData> RootData::allocator;
        


void RootData::free()
{
    allocator.free(this);
}



void RootData::reset()
{
}

RootData::~RootData()
{
}

RootData * RootData::copy() const
{
    RootData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool RootData::instanceOf(int classNo) const
{
    if(ROOT_NO == classNo) return true;
    return BaseObjectData::instanceOf(classNo);
}

void RootData::fillDefaultObjectInstance(RootData& data, std::map<std::string, int>& attr_data)
{
        data.attr_stamp = 0.0;
        data.attr_objtype = "obj";
        data.attr_parent = "root";
    attr_data[ID_ATTR] = ID_FLAG;
    attr_data[PARENT_ATTR] = PARENT_FLAG;
    attr_data[STAMP_ATTR] = STAMP_FLAG;
    attr_data[OBJTYPE_ATTR] = OBJTYPE_FLAG;
    attr_data[NAME_ATTR] = NAME_FLAG;
}

} } // namespace Atlas::Objects
