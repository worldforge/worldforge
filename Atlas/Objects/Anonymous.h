// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011 Erik Ogenvik.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_ENTITY_ANONYMOUS_H
#define ATLAS_OBJECTS_ENTITY_ANONYMOUS_H

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

namespace Atlas { namespace Objects { namespace Entity { 

/** Starting point for entity hierarchy

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class AnonymousData;
typedef SmartPtr<AnonymousData> Anonymous;

static const int ANONYMOUS_NO = 42;

/// \brief Starting point for entity hierarchy.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class AnonymousData : public RootEntityData
{
protected:
    /// Construct a AnonymousData class definition.
    AnonymousData(AnonymousData *defaults = nullptr) : 
        RootEntityData((RootEntityData*)defaults)
    {
        m_class_no = ANONYMOUS_NO;
    }
    /// Default destructor.
    ~AnonymousData() override = default;

public:
    /// Set the type of this object.
    void setType(const std::string &, int);

    /// Copy this object.
    AnonymousData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == ANONYMOUS_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AnonymousData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AnonymousData& data, std::map<std::string, uint32_t>& attr_data);
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ANONYMOUS_H
