// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#ifndef ATLAS_OBJECTS_OPERATION_OPERATION_H
#define ATLAS_OBJECTS_OPERATION_OPERATION_H

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Generic.h>


namespace Atlas::Objects::Operation {

/** This is base operator for operations that might have effects.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class ActionData;
typedef SmartPtr<ActionData> Action;

static const int ACTION_NO = 11;

/// \brief This is base operator for operations that might have effects..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class ActionData : public RootOperationData
{
protected:
    /// Construct a ActionData class definition.
    explicit ActionData(ActionData *defaults = nullptr) : 
        RootOperationData((RootOperationData*)defaults)
    {
        m_class_no = ACTION_NO;
    }
    /// Default destructor.
    ~ActionData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root_operation";
    // The default parent type for this object
    static constexpr const char* default_parent = "action";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    ActionData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ActionData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ActionData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Create new things from nothing using this operator.

Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.

*/

class CreateData;
typedef SmartPtr<CreateData> Create;

static const int CREATE_NO = 12;

/// \brief Create new things from nothing using this operator..
///
/** Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.
 */
class CreateData : public ActionData
{
protected:
    /// Construct a CreateData class definition.
    explicit CreateData(CreateData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = CREATE_NO;
    }
    /// Default destructor.
    ~CreateData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "create";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    CreateData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<CreateData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(CreateData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Delete something.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class DeleteData;
typedef SmartPtr<DeleteData> Delete;

static const int DELETE_NO = 13;

/// \brief Delete something..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class DeleteData : public ActionData
{
protected:
    /// Construct a DeleteData class definition.
    explicit DeleteData(DeleteData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = DELETE_NO;
    }
    /// Default destructor.
    ~DeleteData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "delete";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    DeleteData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<DeleteData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(DeleteData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Sets attributes for existing entity.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class SetData;
typedef SmartPtr<SetData> Set;

static const int SET_NO = 14;

/// \brief Sets attributes for existing entity..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class SetData : public ActionData
{
protected:
    /// Construct a SetData class definition.
    explicit SetData(SetData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = SET_NO;
    }
    /// Default destructor.
    ~SetData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "set";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    SetData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SetData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SetData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Sets attributes for existing entity.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class AffectData;
typedef SmartPtr<AffectData> Affect;

static const int AFFECT_NO = 15;

/// \brief Sets attributes for existing entity..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class AffectData : public SetData
{
protected:
    /// Construct a AffectData class definition.
    explicit AffectData(AffectData *defaults = nullptr) : 
        SetData((SetData*)defaults)
    {
        m_class_no = AFFECT_NO;
    }
    /// Default destructor.
    ~AffectData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "set";
    // The default parent type for this object
    static constexpr const char* default_parent = "affect";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    AffectData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AffectData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AffectData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Operation for when one entity hits another.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class HitData;
typedef SmartPtr<HitData> Hit;

static const int HIT_NO = 16;

/// \brief Operation for when one entity hits another..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class HitData : public AffectData
{
protected:
    /// Construct a HitData class definition.
    explicit HitData(HitData *defaults = nullptr) : 
        AffectData((AffectData*)defaults)
    {
        m_class_no = HIT_NO;
    }
    /// Default destructor.
    ~HitData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "affect";
    // The default parent type for this object
    static constexpr const char* default_parent = "hit";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    HitData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<HitData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(HitData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Change position

More about <a href="move.html">movement here</a>

*/

class MoveData;
typedef SmartPtr<MoveData> Move;

static const int MOVE_NO = 17;

/// \brief Change position.
///
/** More about <a href="move.html">movement here</a>
 */
class MoveData : public SetData
{
protected:
    /// Construct a MoveData class definition.
    explicit MoveData(MoveData *defaults = nullptr) : 
        SetData((SetData*)defaults)
    {
        m_class_no = MOVE_NO;
    }
    /// Default destructor.
    ~MoveData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "set";
    // The default parent type for this object
    static constexpr const char* default_parent = "move";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    MoveData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<MoveData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(MoveData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Attach a tool to the character entity at a pre-defined location so that the character can use it.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class WieldData;
typedef SmartPtr<WieldData> Wield;

static const int WIELD_NO = 18;

/// \brief Attach a tool to the character entity at a pre-defined location so that the character can use it..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class WieldData : public SetData
{
protected:
    /// Construct a WieldData class definition.
    explicit WieldData(WieldData *defaults = nullptr) : 
        SetData((SetData*)defaults)
    {
        m_class_no = WIELD_NO;
    }
    /// Default destructor.
    ~WieldData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "set";
    // The default parent type for this object
    static constexpr const char* default_parent = "wield";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    WieldData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<WieldData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(WieldData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Generic operation for getting info about things.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class GetData;
typedef SmartPtr<GetData> Get;

static const int GET_NO = 19;

/// \brief Generic operation for getting info about things..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class GetData : public ActionData
{
protected:
    /// Construct a GetData class definition.
    explicit GetData(GetData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = GET_NO;
    }
    /// Default destructor.
    ~GetData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "get";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    GetData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GetData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(GetData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Generic base operation for perceiving things by eyes, ears, etc...

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class PerceiveData;
typedef SmartPtr<PerceiveData> Perceive;

static const int PERCEIVE_NO = 20;

/// \brief Generic base operation for perceiving things by eyes, ears, etc....
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class PerceiveData : public GetData
{
protected:
    /// Construct a PerceiveData class definition.
    explicit PerceiveData(PerceiveData *defaults = nullptr) : 
        GetData((GetData*)defaults)
    {
        m_class_no = PERCEIVE_NO;
    }
    /// Default destructor.
    ~PerceiveData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "get";
    // The default parent type for this object
    static constexpr const char* default_parent = "perceive";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    PerceiveData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<PerceiveData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(PerceiveData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Looking at something

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class LookData;
typedef SmartPtr<LookData> Look;

static const int LOOK_NO = 21;

/// \brief Looking at something.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class LookData : public PerceiveData
{
protected:
    /// Construct a LookData class definition.
    explicit LookData(LookData *defaults = nullptr) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = LOOK_NO;
    }
    /// Default destructor.
    ~LookData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perceive";
    // The default parent type for this object
    static constexpr const char* default_parent = "look";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    LookData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<LookData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(LookData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Listen (something)

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class ListenData;
typedef SmartPtr<ListenData> Listen;

static const int LISTEN_NO = 22;

/// \brief Listen (something).
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class ListenData : public PerceiveData
{
protected:
    /// Construct a ListenData class definition.
    explicit ListenData(ListenData *defaults = nullptr) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = LISTEN_NO;
    }
    /// Default destructor.
    ~ListenData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perceive";
    // The default parent type for this object
    static constexpr const char* default_parent = "listen";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    ListenData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ListenData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ListenData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Sniff something

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class SniffData;
typedef SmartPtr<SniffData> Sniff;

static const int SNIFF_NO = 23;

/// \brief Sniff something.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class SniffData : public PerceiveData
{
protected:
    /// Construct a SniffData class definition.
    explicit SniffData(SniffData *defaults = nullptr) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = SNIFF_NO;
    }
    /// Default destructor.
    ~SniffData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perceive";
    // The default parent type for this object
    static constexpr const char* default_parent = "sniff";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    SniffData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SniffData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SniffData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Touch something

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class TouchData;
typedef SmartPtr<TouchData> Touch;

static const int TOUCH_NO = 24;

/// \brief Touch something.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class TouchData : public PerceiveData
{
protected:
    /// Construct a TouchData class definition.
    explicit TouchData(TouchData *defaults = nullptr) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = TOUCH_NO;
    }
    /// Default destructor.
    ~TouchData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perceive";
    // The default parent type for this object
    static constexpr const char* default_parent = "touch";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    TouchData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<TouchData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(TouchData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Operation for logging into server

For more about <a href="login.html">out of game atlas here</a>

*/

class LoginData;
typedef SmartPtr<LoginData> Login;

static const int LOGIN_NO = 25;

/// \brief Operation for logging into server.
///
/** For more about <a href="login.html">out of game atlas here</a>
 */
class LoginData : public GetData
{
protected:
    /// Construct a LoginData class definition.
    explicit LoginData(LoginData *defaults = nullptr) : 
        GetData((GetData*)defaults)
    {
        m_class_no = LOGIN_NO;
    }
    /// Default destructor.
    ~LoginData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "get";
    // The default parent type for this object
    static constexpr const char* default_parent = "login";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    LoginData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<LoginData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(LoginData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Operation for logging out

For more about <a href="login.html">out of game atlas here</a>

*/

class LogoutData;
typedef SmartPtr<LogoutData> Logout;

static const int LOGOUT_NO = 26;

/// \brief Operation for logging out.
///
/** For more about <a href="login.html">out of game atlas here</a>
 */
class LogoutData : public LoginData
{
protected:
    /// Construct a LogoutData class definition.
    explicit LogoutData(LogoutData *defaults = nullptr) : 
        LoginData((LoginData*)defaults)
    {
        m_class_no = LOGOUT_NO;
    }
    /// Default destructor.
    ~LogoutData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "login";
    // The default parent type for this object
    static constexpr const char* default_parent = "logout";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    LogoutData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<LogoutData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(LogoutData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Base operator for all kind of communication.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class CommunicateData;
typedef SmartPtr<CommunicateData> Communicate;

static const int COMMUNICATE_NO = 27;

/// \brief Base operator for all kind of communication..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class CommunicateData : public ActionData
{
protected:
    /// Construct a CommunicateData class definition.
    explicit CommunicateData(CommunicateData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = COMMUNICATE_NO;
    }
    /// Default destructor.
    ~CommunicateData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "communicate";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    CommunicateData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<CommunicateData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(CommunicateData& data, std::map<std::string, uint32_t>& attr_data);
};


/** used for talking

need to relook at that 'Magic voice that steps on something crunchy and makes magic voice which...' -IRC log

*/

class TalkData;
typedef SmartPtr<TalkData> Talk;

static const int TALK_NO = 28;

/// \brief used for talking.
///
/** need to relook at that 'Magic voice that steps on something crunchy and makes magic voice which...' -IRC log
 */
class TalkData : public CommunicateData
{
protected:
    /// Construct a TalkData class definition.
    explicit TalkData(TalkData *defaults = nullptr) : 
        CommunicateData((CommunicateData*)defaults)
    {
        m_class_no = TALK_NO;
    }
    /// Default destructor.
    ~TalkData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "communicate";
    // The default parent type for this object
    static constexpr const char* default_parent = "talk";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    TalkData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<TalkData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(TalkData& data, std::map<std::string, uint32_t>& attr_data);
};


/** When something is not yet implemented in server, then character can pretend to do something ;-)

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class ImaginaryData;
typedef SmartPtr<ImaginaryData> Imaginary;

static const int IMAGINARY_NO = 29;

/// \brief When something is not yet implemented in server, then character can pretend to do something ;-).
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class ImaginaryData : public ActionData
{
protected:
    /// Construct a ImaginaryData class definition.
    explicit ImaginaryData(ImaginaryData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = IMAGINARY_NO;
    }
    /// Default destructor.
    ~ImaginaryData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "imaginary";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    ImaginaryData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ImaginaryData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ImaginaryData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Use a currently wielded tool.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class UseData;
typedef SmartPtr<UseData> Use;

static const int USE_NO = 30;

/// \brief Use a currently wielded tool..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class UseData : public ActionData
{
protected:
    /// Construct a UseData class definition.
    explicit UseData(UseData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = USE_NO;
    }
    /// Default destructor.
    ~UseData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "use";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    UseData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<UseData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(UseData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Activities performed by entities. This is mainly meant for actions that should be communicated to other entities (like 'digging' or 'twirling')

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class ActivityData;
typedef SmartPtr<ActivityData> Activity;

static const int ACTIVITY_NO = 31;

/// \brief Activities performed by entities. This is mainly meant for actions that should be communicated to other entities (like 'digging' or 'twirling').
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class ActivityData : public ActionData
{
protected:
    /// Construct a ActivityData class definition.
    explicit ActivityData(ActivityData *defaults = nullptr) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = ACTIVITY_NO;
    }
    /// Default destructor.
    ~ActivityData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "action";
    // The default parent type for this object
    static constexpr const char* default_parent = "activity";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    ActivityData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ActivityData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ActivityData& data, std::map<std::string, uint32_t>& attr_data);
};


/** This is base operator for operations that tell you info about objects or events.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class InfoData;
typedef SmartPtr<InfoData> Info;

static const int INFO_NO = 32;

/// \brief This is base operator for operations that tell you info about objects or events..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class InfoData : public RootOperationData
{
protected:
    /// Construct a InfoData class definition.
    explicit InfoData(InfoData *defaults = nullptr) : 
        RootOperationData((RootOperationData*)defaults)
    {
        m_class_no = INFO_NO;
    }
    /// Default destructor.
    ~InfoData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root_operation";
    // The default parent type for this object
    static constexpr const char* default_parent = "info";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    InfoData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<InfoData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(InfoData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character perceives something.

Base operator for all kind of perceptions

*/

class PerceptionData;
typedef SmartPtr<PerceptionData> Perception;

static const int PERCEPTION_NO = 33;

/// \brief Character perceives something..
///
/** Base operator for all kind of perceptions
 */
class PerceptionData : public InfoData
{
protected:
    /// Construct a PerceptionData class definition.
    explicit PerceptionData(PerceptionData *defaults = nullptr) : 
        InfoData((InfoData*)defaults)
    {
        m_class_no = PERCEPTION_NO;
    }
    /// Default destructor.
    ~PerceptionData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "info";
    // The default parent type for this object
    static constexpr const char* default_parent = "perception";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    PerceptionData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<PerceptionData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(PerceptionData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character failed to interact with another entity because it does not exist.

Base operator for all kind of perceptions

*/

class UnseenData;
typedef SmartPtr<UnseenData> Unseen;

static const int UNSEEN_NO = 34;

/// \brief Character failed to interact with another entity because it does not exist..
///
/** Base operator for all kind of perceptions
 */
class UnseenData : public PerceptionData
{
protected:
    /// Construct a UnseenData class definition.
    explicit UnseenData(UnseenData *defaults = nullptr) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = UNSEEN_NO;
    }
    /// Default destructor.
    ~UnseenData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perception";
    // The default parent type for this object
    static constexpr const char* default_parent = "unseen";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    UnseenData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<UnseenData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(UnseenData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character sees something

Base operator for all kind of perceptions

*/

class SightData;
typedef SmartPtr<SightData> Sight;

static const int SIGHT_NO = 35;

/// \brief Character sees something.
///
/** Base operator for all kind of perceptions
 */
class SightData : public PerceptionData
{
protected:
    /// Construct a SightData class definition.
    explicit SightData(SightData *defaults = nullptr) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = SIGHT_NO;
    }
    /// Default destructor.
    ~SightData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perception";
    // The default parent type for this object
    static constexpr const char* default_parent = "sight";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    SightData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SightData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SightData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character sees something appearing: it literally appears or has it come in visible range

Base operator for all kind of perceptions

*/

class AppearanceData;
typedef SmartPtr<AppearanceData> Appearance;

static const int APPEARANCE_NO = 36;

/// \brief Character sees something appearing: it literally appears or has it come in visible range.
///
/** Base operator for all kind of perceptions
 */
class AppearanceData : public SightData
{
protected:
    /// Construct a AppearanceData class definition.
    explicit AppearanceData(AppearanceData *defaults = nullptr) : 
        SightData((SightData*)defaults)
    {
        m_class_no = APPEARANCE_NO;
    }
    /// Default destructor.
    ~AppearanceData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "sight";
    // The default parent type for this object
    static constexpr const char* default_parent = "appearance";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    AppearanceData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AppearanceData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AppearanceData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character sees something disappearing: it literally disappears or has it gone too far to be visible

Base operator for all kind of perceptions

*/

class DisappearanceData;
typedef SmartPtr<DisappearanceData> Disappearance;

static const int DISAPPEARANCE_NO = 37;

/// \brief Character sees something disappearing: it literally disappears or has it gone too far to be visible.
///
/** Base operator for all kind of perceptions
 */
class DisappearanceData : public SightData
{
protected:
    /// Construct a DisappearanceData class definition.
    explicit DisappearanceData(DisappearanceData *defaults = nullptr) : 
        SightData((SightData*)defaults)
    {
        m_class_no = DISAPPEARANCE_NO;
    }
    /// Default destructor.
    ~DisappearanceData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "sight";
    // The default parent type for this object
    static constexpr const char* default_parent = "disappearance";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    DisappearanceData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<DisappearanceData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(DisappearanceData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character hears something

Base operator for all kind of perceptions

*/

class SoundData;
typedef SmartPtr<SoundData> Sound;

static const int SOUND_NO = 38;

/// \brief Character hears something.
///
/** Base operator for all kind of perceptions
 */
class SoundData : public PerceptionData
{
protected:
    /// Construct a SoundData class definition.
    explicit SoundData(SoundData *defaults = nullptr) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = SOUND_NO;
    }
    /// Default destructor.
    ~SoundData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perception";
    // The default parent type for this object
    static constexpr const char* default_parent = "sound";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    SoundData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SoundData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SoundData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character smells something

Base operator for all kind of perceptions

*/

class SmellData;
typedef SmartPtr<SmellData> Smell;

static const int SMELL_NO = 39;

/// \brief Character smells something.
///
/** Base operator for all kind of perceptions
 */
class SmellData : public PerceptionData
{
protected:
    /// Construct a SmellData class definition.
    explicit SmellData(SmellData *defaults = nullptr) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = SMELL_NO;
    }
    /// Default destructor.
    ~SmellData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perception";
    // The default parent type for this object
    static constexpr const char* default_parent = "smell";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    SmellData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SmellData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SmellData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Character feels something (with fingers usually)

Base operator for all kind of perceptions

*/

class FeelData;
typedef SmartPtr<FeelData> Feel;

static const int FEEL_NO = 40;

/// \brief Character feels something (with fingers usually).
///
/** Base operator for all kind of perceptions
 */
class FeelData : public PerceptionData
{
protected:
    /// Construct a FeelData class definition.
    explicit FeelData(FeelData *defaults = nullptr) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = FEEL_NO;
    }
    /// Default destructor.
    ~FeelData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "perception";
    // The default parent type for this object
    static constexpr const char* default_parent = "feel";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    FeelData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<FeelData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(FeelData& data, std::map<std::string, uint32_t>& attr_data);
};


/** Something went wrong

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class ErrorData;
typedef SmartPtr<ErrorData> Error;

static const int ERROR_NO = 41;

/// \brief Something went wrong.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class ErrorData : public InfoData
{
protected:
    /// Construct a ErrorData class definition.
    explicit ErrorData(ErrorData *defaults = nullptr) : 
        InfoData((InfoData*)defaults)
    {
        m_class_no = ERROR_NO;
    }
    /// Default destructor.
    ~ErrorData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "info";
    // The default parent type for this object
    static constexpr const char* default_parent = "error";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    ErrorData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ErrorData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ErrorData& data, std::map<std::string, uint32_t>& attr_data);
};


/** An operation used to signal to clients when things such as types have changed.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.

*/

class ChangeData;
typedef SmartPtr<ChangeData> Change;

static const int CHANGE_NO = 42;

/// \brief An operation used to signal to clients when things such as types have changed..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hierarchy. refno refers
    to the operation that this is a reply for.
 */
class ChangeData : public InfoData
{
protected:
    /// Construct a ChangeData class definition.
    explicit ChangeData(ChangeData *defaults = nullptr) : 
        InfoData((InfoData*)defaults)
    {
        m_class_no = CHANGE_NO;
    }
    /// Default destructor.
    ~ChangeData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "info";
    // The default parent type for this object
    static constexpr const char* default_parent = "change";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    ChangeData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ChangeData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ChangeData& data, std::map<std::string, uint32_t>& attr_data);
};

} // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_OPERATION_H
