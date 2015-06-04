#ifndef ERIS_CUSTOMENTITIES_H_
#define ERIS_CUSTOMENTITIES_H_

#include <Atlas/Objects/Entity.h>

namespace Atlas { namespace Objects { namespace Entity {

class SysData;
typedef SmartPtr<SysData> Sys;

extern int SYS_NO;

/// \brief Privileged accounts which aren't persisted on the server.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class SysData : public AccountData
{
protected:
    /// Construct a AdminData class definition.
    SysData(SysData *defaults = NULL) :
        AccountData((AccountData*)defaults)
    {
        m_class_no = SYS_NO;
    }
    /// Default destructor.
    virtual ~SysData();

public:
    /// Copy this object.
    virtual SysData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == SYS_NO) current_class = -1; AccountData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static ::Atlas::Objects::Allocator<SysData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(SysData& data, std::map<std::string, int>& attr_data);

};

} } }

#endif /* ERIS_CUSTOMENTITIES_H_ */
