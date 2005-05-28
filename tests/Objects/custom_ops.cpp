// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Operation::SetData;

class WieldData;
typedef Atlas::Objects::SmartPtr<WieldData> Wield;

int WIELD_NO = -1;

class WieldData : public SetData
{
protected:
    /// Construct a WieldData class definition.
    WieldData(WieldData *defaults = NULL) : SetData((SetData*)defaults)
    {
        m_class_no = WIELD_NO;
    }
    /// Default destructor.
    virtual ~WieldData();

public:
    /// Copy this object.
    virtual WieldData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == WIELD_NO) current_class = -1; SetData::iterate(current_class, attr);}

    //freelist related things
public:
    static WieldData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual WieldData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static WieldData *getDefaultObjectInstance();
private:
    static WieldData *defaults_WieldData;
    static WieldData *begin_WieldData;
};

WieldData::~WieldData()
{
}

WieldData * WieldData::copy() const
{
    WieldData * copied = WieldData::alloc();
    *copied = *this;
    return copied;
}

bool WieldData::instanceOf(int classNo) const
{
    if(WIELD_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

//freelist related methods specific to this class
WieldData *WieldData::defaults_WieldData = 0;
WieldData *WieldData::begin_WieldData = 0;

WieldData *WieldData::alloc()
{
    if(begin_WieldData) {
        WieldData *res = begin_WieldData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_WieldData = (WieldData *)begin_WieldData->m_next;
        return res;
    }
    return new WieldData(WieldData::getDefaultObjectInstance());
}

void WieldData::free()
{
    m_next = begin_WieldData;
    begin_WieldData = this;
}


WieldData *WieldData::getDefaultObjectInstance()
{
    if (defaults_WieldData == 0) {
        defaults_WieldData = new WieldData;
        defaults_WieldData->attr_objtype = "op";
        defaults_WieldData->attr_serialno = 0;
        defaults_WieldData->attr_refno = 0;
        defaults_WieldData->attr_seconds = 0.0;
        defaults_WieldData->attr_future_seconds = 0.0;
        defaults_WieldData->attr_stamp = 0.0;
        defaults_WieldData->attr_parents = std::list<std::string>(1, "wield");
    }
    return defaults_WieldData;
}

WieldData *WieldData::getDefaultObject()
{
    return WieldData::getDefaultObjectInstance();
}

int main(int argc, char ** argv)
{
    WIELD_NO = Atlas::Objects::Factories::instance()->addFactory("wield", &Atlas::Objects::factory<WieldData>);

    Wield op;
}
