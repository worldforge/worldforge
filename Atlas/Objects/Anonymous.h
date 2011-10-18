// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
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

static const int ANONYMOUS_NO = 40;

/// \brief Starting point for entity hierarchy.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class AnonymousData : public RootEntityData
{
protected:
    /// Construct a AnonymousData class definition.
    AnonymousData(AnonymousData *defaults = NULL) : 
        RootEntityData((RootEntityData*)defaults)
    {
        m_class_no = ANONYMOUS_NO;
    }
    /// Default destructor.
    virtual ~AnonymousData();

public:
    /// Set the type of this object.
    void setType(const std::string &, int);

    /// Copy this object.
    virtual AnonymousData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ANONYMOUS_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AnonymousData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(AnonymousData& data, std::map<std::string, int>& attr_data);
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ANONYMOUS_H
