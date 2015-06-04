#include "CustomEntities.h"

namespace Atlas { namespace Objects { namespace Entity {

int SYS_NO = -1;

SysData::~SysData()
{
}

SysData * SysData::copy() const
{
    SysData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SysData::instanceOf(int classNo) const
{
    if(SYS_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}


Allocator<SysData> SysData::allocator;

void SysData::free()
{
    allocator.free(this);
}

void SysData::reset()
{
    AccountData::reset();
}

void SysData::fillDefaultObjectInstance(SysData& data, std::map<std::string, int>& attr_data)
{
    data.attr_objtype = "obj";
    data.attr_pos.clear();
    data.attr_pos.push_back(0.0);
    data.attr_pos.push_back(0.0);
    data.attr_pos.push_back(0.0);
    data.attr_velocity.clear();
    data.attr_velocity.push_back(0.0);
    data.attr_velocity.push_back(0.0);
    data.attr_velocity.push_back(0.0);
    data.attr_stamp_contains = 0.0;
    data.attr_stamp = 0.0;
    data.attr_parents = std::list<std::string>(1, "sys");

}

} } }
