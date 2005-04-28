// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
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

static const int ANONYMOUS_NO = 37;

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
    /// Copy this object.
    virtual AnonymousData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ANONYMOUS_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

    //freelist related things
public:
    static AnonymousData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual AnonymousData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static AnonymousData *getDefaultObjectInstance();
private:
    static AnonymousData *defaults_AnonymousData;
    static AnonymousData *begin_AnonymousData;
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ANONYMOUS_H
