// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Generic.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<GenericData> GenericData::allocator;
        


void GenericData::free()
{
    allocator.free(this);
}



void GenericData::reset()
{
    RootOperationData::reset();
}

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
    GenericData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool GenericData::instanceOf(int classNo) const
{
    if(GENERIC_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

void GenericData::fillDefaultObjectInstance(GenericData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
    RootOperationData::allocator.getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
