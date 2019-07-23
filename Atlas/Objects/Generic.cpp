// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2019 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

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

void GenericData::setType(const std::string & name, int no)
{
    setParent(name);
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

void GenericData::fillDefaultObjectInstance(GenericData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
}

} } } // namespace Atlas::Objects::Operation
