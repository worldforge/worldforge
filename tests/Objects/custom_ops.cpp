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

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static ::Atlas::Objects::Allocator<WieldData> allocator;

private:
    static void fillDefaultObjectInstance(WieldData& data, std::map<std::string, uint32_t>& attr_data);
};


::Atlas::Objects::Allocator<WieldData> WieldData::allocator;

WieldData::~WieldData()
{
}

WieldData * WieldData::copy() const
{
    WieldData * copied = WieldData::allocator.alloc();
    *copied = *this;
    return copied;
}

bool WieldData::instanceOf(int classNo) const
{
    if(WIELD_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

void WieldData::fillDefaultObjectInstance(WieldData& data, std::map<std::string, uint32_t>& attr_data)
{
    data.attr_objtype = "op";
    data.attr_serialno = 0;
    data.attr_refno = 0;
    data.attr_seconds = 0.0;
    data.attr_future_seconds = 0.0;
    data.attr_stamp = 0.0;
    data.attr_parent = "wield";
}


int main(int argc, char ** argv)
{
	Atlas::Objects::Factories factories;
    WIELD_NO = factories.addFactory("wield", &Atlas::Objects::factory<WieldData>, &Atlas::Objects::defaultInstance<WieldData>);

    Wield op;
}
