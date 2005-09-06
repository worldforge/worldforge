// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Generic.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

GenericData::~GenericData()
{
}

void GenericData::setType(const std::string & name, int no)
{
    setParents(std::list<std::string>(1, name));
    m_class_no = no;
}

GenericData * GenericData::copy() const
{
    GenericData * copied = GenericData::alloc();
    *copied = *this;
    return copied;
}

bool GenericData::instanceOf(int classNo) const
{
    if(GENERIC_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

//freelist related methods specific to this class
GenericData *GenericData::defaults_GenericData = 0;
GenericData *GenericData::begin_GenericData = 0;

GenericData *GenericData::alloc()
{
    if(begin_GenericData) {
        GenericData *res = begin_GenericData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_GenericData = (GenericData *)begin_GenericData->m_next;
        return res;
    }
    return new GenericData(GenericData::getDefaultObjectInstance());
}

void GenericData::free()
{
    m_next = begin_GenericData;
    begin_GenericData = this;
}


GenericData *GenericData::getDefaultObjectInstance()
{
    if (defaults_GenericData == 0) {
        defaults_GenericData = new GenericData;
        defaults_GenericData->attr_serialno = 0;
        defaults_GenericData->attr_refno = 0;
        defaults_GenericData->attr_seconds = 0.0;
        defaults_GenericData->attr_future_seconds = 0.0;
        defaults_GenericData->attr_stamp = 0.0;
        RootOperationData::getDefaultObjectInstance();
    }
    return defaults_GenericData;
}

GenericData *GenericData::getDefaultObject()
{
    return GenericData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
