// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#ifndef ATLAS_OBJECTS_OPERATION_GENERIC_H
#define ATLAS_OBJECTS_OPERATION_GENERIC_H

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

namespace Atlas::Objects::Operation {

/** Base operation for all operators

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class GenericData;
typedef SmartPtr<GenericData> Generic;

static const int GENERIC_NO = 44;

/// \brief Base operation for all operators.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class GenericData : public RootOperationData
{
protected:
    /// Construct a GenericData class definition.
    explicit GenericData(GenericData *defaults = nullptr) : 
        RootOperationData((RootOperationData*)defaults)
    {
        m_class_no = GENERIC_NO;
    }
    /// Default destructor.
    ~GenericData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root_operation";
    // The default parent type for this object
    static constexpr const char* default_parent = "generic";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Set the type of this object.
    void setType(const std::string &, int);

    /// Copy this object.
    GenericData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GenericData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(GenericData& data, std::map<std::string, uint32_t>& attr_data);
};

} // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_GENERIC_H
