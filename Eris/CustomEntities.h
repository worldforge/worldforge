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

    //freelist related things
public:
    static SysData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual SysData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static SysData *getDefaultObjectInstance();
private:
    static SysData *defaults_SysData;
    static SysData *begin_SysData;
};

} } }

#endif /* ERIS_CUSTOMENTITIES_H_ */
