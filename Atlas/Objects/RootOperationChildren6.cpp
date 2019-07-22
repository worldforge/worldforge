// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011 Erik Ogenvik.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<ErrorData> ErrorData::allocator;
        


void ErrorData::free()
{
    allocator.free(this);
}



void ErrorData::reset()
{
    InfoData::reset();
}

ErrorData * ErrorData::copy() const
{
    ErrorData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool ErrorData::instanceOf(int classNo) const
{
    if(ERROR_NO == classNo) return true;
    return InfoData::instanceOf(classNo);
}

void ErrorData::fillDefaultObjectInstance(ErrorData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "error";
}

} } } // namespace Atlas::Objects::Operation
