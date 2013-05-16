#include "CustomEntities.h"

namespace Atlas { namespace Objects { namespace Entity {

int SYS_NO = -1;

SysData::~SysData()
{
}

SysData * SysData::copy() const
{
    SysData * copied = SysData::alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SysData::instanceOf(int classNo) const
{
    if(SYS_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

//freelist related methods specific to this class
SysData *SysData::defaults_SysData = 0;
SysData *SysData::begin_SysData = 0;

SysData *SysData::alloc()
{
    if(begin_SysData) {
        SysData *res = begin_SysData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_SysData = (SysData *)begin_SysData->m_next;
        return res;
    }
    return new SysData(SysData::getDefaultObjectInstance());
}

void SysData::free()
{
    m_next = begin_SysData;
    begin_SysData = this;
}


SysData *SysData::getDefaultObjectInstance()
{
    if (defaults_SysData == 0) {
        defaults_SysData = new SysData;
        defaults_SysData->attr_objtype = "obj";
        defaults_SysData->attr_pos.clear();
        defaults_SysData->attr_pos.push_back(0.0);
        defaults_SysData->attr_pos.push_back(0.0);
        defaults_SysData->attr_pos.push_back(0.0);
        defaults_SysData->attr_velocity.clear();
        defaults_SysData->attr_velocity.push_back(0.0);
        defaults_SysData->attr_velocity.push_back(0.0);
        defaults_SysData->attr_velocity.push_back(0.0);
        defaults_SysData->attr_stamp_contains = 0.0;
        defaults_SysData->attr_stamp = 0.0;
        defaults_SysData->attr_parents = std::list<std::string>(1, "sys");
        AccountData::getDefaultObjectInstance();
    }
    return defaults_SysData;
}

SysData *SysData::getDefaultObject()
{
    return SysData::getDefaultObjectInstance();
}

} } }
