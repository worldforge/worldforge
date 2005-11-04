// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/RootOperation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

const std::string SERIALNO_ATTR = "serialno";
const std::string REFNO_ATTR = "refno";
const std::string FROM_ATTR = "from";
const std::string TO_ATTR = "to";
const std::string SECONDS_ATTR = "seconds";
const std::string FUTURE_SECONDS_ATTR = "future_seconds";
const std::string ARGS_ATTR = "args";

int RootOperationData::getAttrClass(const std::string& name) const
{
    if (attr_flags_RootOperationData->find(name) != attr_flags_RootOperationData->end()) {
        return ROOT_OPERATION_NO;
    }
    return RootData::getAttrClass(name);
}

int RootOperationData::getAttrFlag(const std::string& name) const
{
    std::map<std::string, int>::const_iterator I = attr_flags_RootOperationData->find(name);
    if (I != attr_flags_RootOperationData->end()) {
        return I->second;
    }
    return RootData::getAttrFlag(name);
}

int RootOperationData::copyAttr(const std::string& name, Element & attr) const
{
    if (name == SERIALNO_ATTR) { attr = getSerialno(); return 0; }
    if (name == REFNO_ATTR) { attr = getRefno(); return 0; }
    if (name == FROM_ATTR) { attr = getFrom(); return 0; }
    if (name == TO_ATTR) { attr = getTo(); return 0; }
    if (name == SECONDS_ATTR) { attr = getSeconds(); return 0; }
    if (name == FUTURE_SECONDS_ATTR) { attr = getFutureSeconds(); return 0; }
    if (name == ARGS_ATTR) { attr = getArgsAsList(); return 0; }
    return RootData::copyAttr(name, attr);
}

void RootOperationData::setAttr(const std::string& name, const Element& attr)
{
    if (name == SERIALNO_ATTR) { setSerialno(attr.asInt()); return; }
    if (name == REFNO_ATTR) { setRefno(attr.asInt()); return; }
    if (name == FROM_ATTR) { setFrom(attr.asString()); return; }
    if (name == TO_ATTR) { setTo(attr.asString()); return; }
    if (name == SECONDS_ATTR) { setSeconds(attr.asFloat()); return; }
    if (name == FUTURE_SECONDS_ATTR) { setFutureSeconds(attr.asFloat()); return; }
    if (name == ARGS_ATTR) { setArgsAsList(attr.asList()); return; }
    RootData::setAttr(name, attr);
}

void RootOperationData::removeAttr(const std::string& name)
{
    if (name == SERIALNO_ATTR)
        { m_attrFlags &= ~SERIALNO_FLAG; return;}
    if (name == REFNO_ATTR)
        { m_attrFlags &= ~REFNO_FLAG; return;}
    if (name == FROM_ATTR)
        { m_attrFlags &= ~FROM_FLAG; return;}
    if (name == TO_ATTR)
        { m_attrFlags &= ~TO_FLAG; return;}
    if (name == SECONDS_ATTR)
        { m_attrFlags &= ~SECONDS_FLAG; return;}
    if (name == FUTURE_SECONDS_ATTR)
        { m_attrFlags &= ~FUTURE_SECONDS_FLAG; return;}
    if (name == ARGS_ATTR)
        { m_attrFlags &= ~ARGS_FLAG; return;}
    RootData::removeAttr(name);
}

inline void RootOperationData::sendSerialno(Atlas::Bridge & b) const
{
    if(m_attrFlags & SERIALNO_FLAG) {
        b.mapIntItem(SERIALNO_ATTR, attr_serialno);
    }
}

inline void RootOperationData::sendRefno(Atlas::Bridge & b) const
{
    if(m_attrFlags & REFNO_FLAG) {
        b.mapIntItem(REFNO_ATTR, attr_refno);
    }
}

inline void RootOperationData::sendFrom(Atlas::Bridge & b) const
{
    if(m_attrFlags & FROM_FLAG) {
        b.mapStringItem(FROM_ATTR, attr_from);
    }
}

inline void RootOperationData::sendTo(Atlas::Bridge & b) const
{
    if(m_attrFlags & TO_FLAG) {
        b.mapStringItem(TO_ATTR, attr_to);
    }
}

inline void RootOperationData::sendSeconds(Atlas::Bridge & b) const
{
    if(m_attrFlags & SECONDS_FLAG) {
        b.mapFloatItem(SECONDS_ATTR, attr_seconds);
    }
}

inline void RootOperationData::sendFutureSeconds(Atlas::Bridge & b) const
{
    if(m_attrFlags & FUTURE_SECONDS_FLAG) {
        b.mapFloatItem(FUTURE_SECONDS_ATTR, attr_future_seconds);
    }
}

inline void RootOperationData::sendArgs(Atlas::Bridge & b) const
{
    if(m_attrFlags & ARGS_FLAG) {
        b.mapListItem(ARGS_ATTR);
        const std::vector<Root> & v = attr_args;
        std::vector<Root>::const_iterator I = v.begin();
        for (; I != v.end(); ++I) {
           b.listMapItem();
           (*I)->sendContents(b);
           b.mapEnd();
        }
        b.listEnd();
    }
}

void RootOperationData::sendContents(Bridge & b) const
{
    sendSerialno(b);
    sendRefno(b);
    sendFrom(b);
    sendTo(b);
    sendSeconds(b);
    sendFutureSeconds(b);
    sendArgs(b);
    RootData::sendContents(b);
}

void RootOperationData::addToMessage(MapType & m) const
{
    RootData::addToMessage(m);
    if(m_attrFlags & SERIALNO_FLAG)
        m[SERIALNO_ATTR] = attr_serialno;
    if(m_attrFlags & REFNO_FLAG)
        m[REFNO_ATTR] = attr_refno;
    if(m_attrFlags & FROM_FLAG)
        m[FROM_ATTR] = attr_from;
    if(m_attrFlags & TO_FLAG)
        m[TO_ATTR] = attr_to;
    if(m_attrFlags & SECONDS_FLAG)
        m[SECONDS_ATTR] = attr_seconds;
    if(m_attrFlags & FUTURE_SECONDS_FLAG)
        m[FUTURE_SECONDS_ATTR] = attr_future_seconds;
    if(m_attrFlags & ARGS_FLAG)
        m[ARGS_ATTR] = getArgsAsList();
    return;
}

void RootOperationData::iterate(int& current_class, std::string& attr) const
{
    // If we've already finished this class, chain to the parent
    if(current_class >= 0 && current_class != ROOT_OPERATION_NO) {
        RootData::iterate(current_class, attr);
        return;
    }

    static const char *attr_list[] = {"serialno","refno","from","to","seconds","future_seconds","args",};
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
        RootData::iterate(current_class, attr); // chain to parent
    }
    else {
        current_class = ROOT_OPERATION_NO;
        attr = attr_list[next_attr];
    }
}

RootOperationData::~RootOperationData()
{
}

RootOperationData * RootOperationData::copy() const
{
    RootOperationData * copied = RootOperationData::alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool RootOperationData::instanceOf(int classNo) const
{
    if(ROOT_OPERATION_NO == classNo) return true;
    return RootData::instanceOf(classNo);
}

//freelist related methods specific to this class
RootOperationData *RootOperationData::defaults_RootOperationData = 0;
RootOperationData *RootOperationData::begin_RootOperationData = 0;

RootOperationData *RootOperationData::alloc()
{
    if(begin_RootOperationData) {
        RootOperationData *res = begin_RootOperationData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_RootOperationData = (RootOperationData *)begin_RootOperationData->m_next;
        return res;
    }
    return new RootOperationData(RootOperationData::getDefaultObjectInstance());
}

void RootOperationData::free()
{
    m_next = begin_RootOperationData;
    begin_RootOperationData = this;
}

std::map<std::string, int> * RootOperationData::attr_flags_RootOperationData = 0;

RootOperationData *RootOperationData::getDefaultObjectInstance()
{
    if (defaults_RootOperationData == 0) {
        defaults_RootOperationData = new RootOperationData;
        defaults_RootOperationData->attr_objtype = "op";
        defaults_RootOperationData->attr_serialno = 0;
        defaults_RootOperationData->attr_refno = 0;
        defaults_RootOperationData->attr_seconds = 0.0;
        defaults_RootOperationData->attr_future_seconds = 0.0;
        defaults_RootOperationData->attr_stamp = 0.0;
        defaults_RootOperationData->attr_parents = std::list<std::string>(1, "root_operation");
        attr_flags_RootOperationData = new std::map<std::string, int>;
        (*attr_flags_RootOperationData)[SERIALNO_ATTR] = SERIALNO_FLAG;
        (*attr_flags_RootOperationData)[REFNO_ATTR] = REFNO_FLAG;
        (*attr_flags_RootOperationData)[FROM_ATTR] = FROM_FLAG;
        (*attr_flags_RootOperationData)[TO_ATTR] = TO_FLAG;
        (*attr_flags_RootOperationData)[SECONDS_ATTR] = SECONDS_FLAG;
        (*attr_flags_RootOperationData)[FUTURE_SECONDS_ATTR] = FUTURE_SECONDS_FLAG;
        (*attr_flags_RootOperationData)[ARGS_ATTR] = ARGS_FLAG;
        RootData::getDefaultObjectInstance();
    }
    return defaults_RootOperationData;
}

RootOperationData *RootOperationData::getDefaultObject()
{
    return RootOperationData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
