// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/Generic.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas::Objects::Operation {

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
    return copyInstance<GenericData>(*this);
}

bool GenericData::instanceOf(int classNo) const
{
    if(GENERIC_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

void GenericData::fillDefaultObjectInstance(GenericData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
}

} // namespace Atlas::Objects::Operation
