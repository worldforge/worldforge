// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_OPERATION_OPERATION_H
#define ATLAS_OBJECTS_OPERATION_OPERATION_H

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Generic.h>


namespace Atlas { namespace Objects { namespace Operation { 

/** This is base operator for operations that might have effects.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class ActionData;
typedef SmartPtr<ActionData> Action;

static const int ACTION_NO = 10;

/// \brief This is base operator for operations that might have effects..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class ActionData : public RootOperationData
{
protected:
    /// Construct a ActionData class definition.
    ActionData(ActionData *defaults = NULL) : 
        RootOperationData((RootOperationData*)defaults)
    {
        m_class_no = ACTION_NO;
    }
    /// Default destructor.
    virtual ~ActionData() = default;

public:
    /// Copy this object.
    ActionData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == ACTION_NO) current_class = -1; RootOperationData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ActionData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ActionData& data, std::map<std::string, int32_t>& attr_data);
};


/** Create new things from nothing using this operator.

Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.

*/

class CreateData;
typedef SmartPtr<CreateData> Create;

static const int CREATE_NO = 11;

/// \brief Create new things from nothing using this operator..
///
/** Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.
 */
class CreateData : public ActionData
{
protected:
    /// Construct a CreateData class definition.
    CreateData(CreateData *defaults = NULL) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = CREATE_NO;
    }
    /// Default destructor.
    virtual ~CreateData() = default;

public:
    /// Copy this object.
    CreateData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == CREATE_NO) current_class = -1; ActionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<CreateData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(CreateData& data, std::map<std::string, int32_t>& attr_data);
};


/** Combine existing objects into new objects.

This is how normal characters create objects.

*/

class CombineData;
typedef SmartPtr<CombineData> Combine;

static const int COMBINE_NO = 12;

/// \brief Combine existing objects into new objects..
///
/** This is how normal characters create objects.
 */
class CombineData : public CreateData
{
protected:
    /// Construct a CombineData class definition.
    CombineData(CombineData *defaults = NULL) : 
        CreateData((CreateData*)defaults)
    {
        m_class_no = COMBINE_NO;
    }
    /// Default destructor.
    virtual ~CombineData() = default;

public:
    /// Copy this object.
    CombineData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == COMBINE_NO) current_class = -1; CreateData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<CombineData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(CombineData& data, std::map<std::string, int32_t>& attr_data);
};


/** Divide existing object into pieces.

One of the pieces might be original object modified.

*/

class DivideData;
typedef SmartPtr<DivideData> Divide;

static const int DIVIDE_NO = 13;

/// \brief Divide existing object into pieces..
///
/** One of the pieces might be original object modified.
 */
class DivideData : public CreateData
{
protected:
    /// Construct a DivideData class definition.
    DivideData(DivideData *defaults = NULL) : 
        CreateData((CreateData*)defaults)
    {
        m_class_no = DIVIDE_NO;
    }
    /// Default destructor.
    virtual ~DivideData() = default;

public:
    /// Copy this object.
    DivideData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == DIVIDE_NO) current_class = -1; CreateData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<DivideData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(DivideData& data, std::map<std::string, int32_t>& attr_data);
};


/** Base operator for all kind of communication.

Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.

*/

class CommunicateData;
typedef SmartPtr<CommunicateData> Communicate;

static const int COMMUNICATE_NO = 14;

/// \brief Base operator for all kind of communication..
///
/** Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.
 */
class CommunicateData : public CreateData
{
protected:
    /// Construct a CommunicateData class definition.
    CommunicateData(CommunicateData *defaults = NULL) : 
        CreateData((CreateData*)defaults)
    {
        m_class_no = COMMUNICATE_NO;
    }
    /// Default destructor.
    virtual ~CommunicateData() = default;

public:
    /// Copy this object.
    CommunicateData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == COMMUNICATE_NO) current_class = -1; CreateData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<CommunicateData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(CommunicateData& data, std::map<std::string, int32_t>& attr_data);
};


/** used for talking

need to relook at that 'Magic voice that steps on something crunchy and makes magic voice which...' -IRC log

*/

class TalkData;
typedef SmartPtr<TalkData> Talk;

static const int TALK_NO = 15;

/// \brief used for talking.
///
/** need to relook at that 'Magic voice that steps on something crunchy and makes magic voice which...' -IRC log
 */
class TalkData : public CommunicateData
{
protected:
    /// Construct a TalkData class definition.
    TalkData(TalkData *defaults = NULL) : 
        CommunicateData((CommunicateData*)defaults)
    {
        m_class_no = TALK_NO;
    }
    /// Default destructor.
    virtual ~TalkData() = default;

public:
    /// Copy this object.
    TalkData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == TALK_NO) current_class = -1; CommunicateData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<TalkData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(TalkData& data, std::map<std::string, int32_t>& attr_data);
};


/** Delete something.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class DeleteData;
typedef SmartPtr<DeleteData> Delete;

static const int DELETE_NO = 16;

/// \brief Delete something..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class DeleteData : public ActionData
{
protected:
    /// Construct a DeleteData class definition.
    DeleteData(DeleteData *defaults = NULL) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = DELETE_NO;
    }
    /// Default destructor.
    virtual ~DeleteData() = default;

public:
    /// Copy this object.
    DeleteData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == DELETE_NO) current_class = -1; ActionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<DeleteData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(DeleteData& data, std::map<std::string, int32_t>& attr_data);
};


/** Sets attributes for existing entity.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class SetData;
typedef SmartPtr<SetData> Set;

static const int SET_NO = 17;

/// \brief Sets attributes for existing entity..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class SetData : public ActionData
{
protected:
    /// Construct a SetData class definition.
    SetData(SetData *defaults = NULL) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = SET_NO;
    }
    /// Default destructor.
    virtual ~SetData() = default;

public:
    /// Copy this object.
    SetData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == SET_NO) current_class = -1; ActionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SetData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SetData& data, std::map<std::string, int32_t>& attr_data);
};


/** Sets attributes for existing entity.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class AffectData;
typedef SmartPtr<AffectData> Affect;

static const int AFFECT_NO = 18;

/// \brief Sets attributes for existing entity..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class AffectData : public SetData
{
protected:
    /// Construct a AffectData class definition.
    AffectData(AffectData *defaults = NULL) : 
        SetData((SetData*)defaults)
    {
        m_class_no = AFFECT_NO;
    }
    /// Default destructor.
    virtual ~AffectData() = default;

public:
    /// Copy this object.
    AffectData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == AFFECT_NO) current_class = -1; SetData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AffectData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AffectData& data, std::map<std::string, int32_t>& attr_data);
};


/** Change position

More about <a href="move.html">movement here</a>

*/

class MoveData;
typedef SmartPtr<MoveData> Move;

static const int MOVE_NO = 19;

/// \brief Change position.
///
/** More about <a href="move.html">movement here</a>
 */
class MoveData : public SetData
{
protected:
    /// Construct a MoveData class definition.
    MoveData(MoveData *defaults = NULL) : 
        SetData((SetData*)defaults)
    {
        m_class_no = MOVE_NO;
    }
    /// Default destructor.
    virtual ~MoveData() = default;

public:
    /// Copy this object.
    MoveData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == MOVE_NO) current_class = -1; SetData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<MoveData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(MoveData& data, std::map<std::string, int32_t>& attr_data);
};


/** Attach a tool to the character entity at a pre-defined location so that the character can use it.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class WieldData;
typedef SmartPtr<WieldData> Wield;

static const int WIELD_NO = 20;

/// \brief Attach a tool to the character entity at a pre-defined location so that the character can use it..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class WieldData : public SetData
{
protected:
    /// Construct a WieldData class definition.
    WieldData(WieldData *defaults = NULL) : 
        SetData((SetData*)defaults)
    {
        m_class_no = WIELD_NO;
    }
    /// Default destructor.
    virtual ~WieldData() = default;

public:
    /// Copy this object.
    WieldData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == WIELD_NO) current_class = -1; SetData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<WieldData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(WieldData& data, std::map<std::string, int32_t>& attr_data);
};


/** Generic operation for getting info about things.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class GetData;
typedef SmartPtr<GetData> Get;

static const int GET_NO = 21;

/// \brief Generic operation for getting info about things..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class GetData : public ActionData
{
protected:
    /// Construct a GetData class definition.
    GetData(GetData *defaults = NULL) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = GET_NO;
    }
    /// Default destructor.
    virtual ~GetData() = default;

public:
    /// Copy this object.
    GetData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == GET_NO) current_class = -1; ActionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GetData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(GetData& data, std::map<std::string, int32_t>& attr_data);
};


/** Generic base operation for perceiving things by eyes, ears, etc...

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class PerceiveData;
typedef SmartPtr<PerceiveData> Perceive;

static const int PERCEIVE_NO = 22;

/// \brief Generic base operation for perceiving things by eyes, ears, etc....
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class PerceiveData : public GetData
{
protected:
    /// Construct a PerceiveData class definition.
    PerceiveData(PerceiveData *defaults = NULL) : 
        GetData((GetData*)defaults)
    {
        m_class_no = PERCEIVE_NO;
    }
    /// Default destructor.
    virtual ~PerceiveData() = default;

public:
    /// Copy this object.
    PerceiveData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == PERCEIVE_NO) current_class = -1; GetData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<PerceiveData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(PerceiveData& data, std::map<std::string, int32_t>& attr_data);
};


/** Looking at something

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class LookData;
typedef SmartPtr<LookData> Look;

static const int LOOK_NO = 23;

/// \brief Looking at something.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class LookData : public PerceiveData
{
protected:
    /// Construct a LookData class definition.
    LookData(LookData *defaults = NULL) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = LOOK_NO;
    }
    /// Default destructor.
    virtual ~LookData() = default;

public:
    /// Copy this object.
    LookData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == LOOK_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<LookData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(LookData& data, std::map<std::string, int32_t>& attr_data);
};


/** Listen (something)

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class ListenData;
typedef SmartPtr<ListenData> Listen;

static const int LISTEN_NO = 24;

/// \brief Listen (something).
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class ListenData : public PerceiveData
{
protected:
    /// Construct a ListenData class definition.
    ListenData(ListenData *defaults = NULL) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = LISTEN_NO;
    }
    /// Default destructor.
    virtual ~ListenData() = default;

public:
    /// Copy this object.
    ListenData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == LISTEN_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ListenData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ListenData& data, std::map<std::string, int32_t>& attr_data);
};


/** Sniff something

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class SniffData;
typedef SmartPtr<SniffData> Sniff;

static const int SNIFF_NO = 25;

/// \brief Sniff something.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class SniffData : public PerceiveData
{
protected:
    /// Construct a SniffData class definition.
    SniffData(SniffData *defaults = NULL) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = SNIFF_NO;
    }
    /// Default destructor.
    virtual ~SniffData() = default;

public:
    /// Copy this object.
    SniffData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == SNIFF_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SniffData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SniffData& data, std::map<std::string, int32_t>& attr_data);
};


/** Touch something

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class TouchData;
typedef SmartPtr<TouchData> Touch;

static const int TOUCH_NO = 26;

/// \brief Touch something.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class TouchData : public PerceiveData
{
protected:
    /// Construct a TouchData class definition.
    TouchData(TouchData *defaults = NULL) : 
        PerceiveData((PerceiveData*)defaults)
    {
        m_class_no = TOUCH_NO;
    }
    /// Default destructor.
    virtual ~TouchData() = default;

public:
    /// Copy this object.
    TouchData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == TOUCH_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<TouchData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(TouchData& data, std::map<std::string, int32_t>& attr_data);
};


/** Operation for logging into server

For more about <a href="login.html">out of game atlas here</a>

*/

class LoginData;
typedef SmartPtr<LoginData> Login;

static const int LOGIN_NO = 27;

/// \brief Operation for logging into server.
///
/** For more about <a href="login.html">out of game atlas here</a>
 */
class LoginData : public GetData
{
protected:
    /// Construct a LoginData class definition.
    LoginData(LoginData *defaults = NULL) : 
        GetData((GetData*)defaults)
    {
        m_class_no = LOGIN_NO;
    }
    /// Default destructor.
    virtual ~LoginData() = default;

public:
    /// Copy this object.
    LoginData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == LOGIN_NO) current_class = -1; GetData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<LoginData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(LoginData& data, std::map<std::string, int32_t>& attr_data);
};


/** Operation for logging out

For more about <a href="login.html">out of game atlas here</a>

*/

class LogoutData;
typedef SmartPtr<LogoutData> Logout;

static const int LOGOUT_NO = 28;

/// \brief Operation for logging out.
///
/** For more about <a href="login.html">out of game atlas here</a>
 */
class LogoutData : public LoginData
{
protected:
    /// Construct a LogoutData class definition.
    LogoutData(LogoutData *defaults = NULL) : 
        LoginData((LoginData*)defaults)
    {
        m_class_no = LOGOUT_NO;
    }
    /// Default destructor.
    virtual ~LogoutData() = default;

public:
    /// Copy this object.
    LogoutData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == LOGOUT_NO) current_class = -1; LoginData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<LogoutData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(LogoutData& data, std::map<std::string, int32_t>& attr_data);
};


/** When something is not yet implemented in server, then character can pretend to do something ;-)

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class ImaginaryData;
typedef SmartPtr<ImaginaryData> Imaginary;

static const int IMAGINARY_NO = 29;

/// \brief When something is not yet implemented in server, then character can pretend to do something ;-).
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class ImaginaryData : public ActionData
{
protected:
    /// Construct a ImaginaryData class definition.
    ImaginaryData(ImaginaryData *defaults = NULL) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = IMAGINARY_NO;
    }
    /// Default destructor.
    virtual ~ImaginaryData() = default;

public:
    /// Copy this object.
    ImaginaryData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == IMAGINARY_NO) current_class = -1; ActionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ImaginaryData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ImaginaryData& data, std::map<std::string, int32_t>& attr_data);
};


/** Use a currently wielded tool.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class UseData;
typedef SmartPtr<UseData> Use;

static const int USE_NO = 30;

/// \brief Use a currently wielded tool..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class UseData : public ActionData
{
protected:
    /// Construct a UseData class definition.
    UseData(UseData *defaults = NULL) : 
        ActionData((ActionData*)defaults)
    {
        m_class_no = USE_NO;
    }
    /// Default destructor.
    virtual ~UseData() = default;

public:
    /// Copy this object.
    UseData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == USE_NO) current_class = -1; ActionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<UseData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(UseData& data, std::map<std::string, int32_t>& attr_data);
};


/** This is base operator for operations that tell you info about objects or events.

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class InfoData;
typedef SmartPtr<InfoData> Info;

static const int INFO_NO = 31;

/// \brief This is base operator for operations that tell you info about objects or events..
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class InfoData : public RootOperationData
{
protected:
    /// Construct a InfoData class definition.
    InfoData(InfoData *defaults = NULL) : 
        RootOperationData((RootOperationData*)defaults)
    {
        m_class_no = INFO_NO;
    }
    /// Default destructor.
    virtual ~InfoData() = default;

public:
    /// Copy this object.
    InfoData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == INFO_NO) current_class = -1; RootOperationData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<InfoData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(InfoData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character perceives something.

Base operator for all kind of perceptions

*/

class PerceptionData;
typedef SmartPtr<PerceptionData> Perception;

static const int PERCEPTION_NO = 32;

/// \brief Character perceives something..
///
/** Base operator for all kind of perceptions
 */
class PerceptionData : public InfoData
{
protected:
    /// Construct a PerceptionData class definition.
    PerceptionData(PerceptionData *defaults = NULL) : 
        InfoData((InfoData*)defaults)
    {
        m_class_no = PERCEPTION_NO;
    }
    /// Default destructor.
    virtual ~PerceptionData() = default;

public:
    /// Copy this object.
    PerceptionData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == PERCEPTION_NO) current_class = -1; InfoData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<PerceptionData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(PerceptionData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character sees something

Base operator for all kind of perceptions

*/

class SightData;
typedef SmartPtr<SightData> Sight;

static const int SIGHT_NO = 33;

/// \brief Character sees something.
///
/** Base operator for all kind of perceptions
 */
class SightData : public PerceptionData
{
protected:
    /// Construct a SightData class definition.
    SightData(SightData *defaults = NULL) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = SIGHT_NO;
    }
    /// Default destructor.
    virtual ~SightData() = default;

public:
    /// Copy this object.
    SightData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == SIGHT_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SightData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SightData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character sees something appearing: it literally appears or has it come in visible range

Base operator for all kind of perceptions

*/

class AppearanceData;
typedef SmartPtr<AppearanceData> Appearance;

static const int APPEARANCE_NO = 34;

/// \brief Character sees something appearing: it literally appears or has it come in visible range.
///
/** Base operator for all kind of perceptions
 */
class AppearanceData : public SightData
{
protected:
    /// Construct a AppearanceData class definition.
    AppearanceData(AppearanceData *defaults = NULL) : 
        SightData((SightData*)defaults)
    {
        m_class_no = APPEARANCE_NO;
    }
    /// Default destructor.
    virtual ~AppearanceData() = default;

public:
    /// Copy this object.
    AppearanceData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == APPEARANCE_NO) current_class = -1; SightData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AppearanceData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AppearanceData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character sees something disappearing: it literally disappears or has it gone too far to be visible

Base operator for all kind of perceptions

*/

class DisappearanceData;
typedef SmartPtr<DisappearanceData> Disappearance;

static const int DISAPPEARANCE_NO = 35;

/// \brief Character sees something disappearing: it literally disappears or has it gone too far to be visible.
///
/** Base operator for all kind of perceptions
 */
class DisappearanceData : public SightData
{
protected:
    /// Construct a DisappearanceData class definition.
    DisappearanceData(DisappearanceData *defaults = NULL) : 
        SightData((SightData*)defaults)
    {
        m_class_no = DISAPPEARANCE_NO;
    }
    /// Default destructor.
    virtual ~DisappearanceData() = default;

public:
    /// Copy this object.
    DisappearanceData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == DISAPPEARANCE_NO) current_class = -1; SightData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<DisappearanceData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(DisappearanceData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character hears something

Base operator for all kind of perceptions

*/

class SoundData;
typedef SmartPtr<SoundData> Sound;

static const int SOUND_NO = 36;

/// \brief Character hears something.
///
/** Base operator for all kind of perceptions
 */
class SoundData : public PerceptionData
{
protected:
    /// Construct a SoundData class definition.
    SoundData(SoundData *defaults = NULL) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = SOUND_NO;
    }
    /// Default destructor.
    virtual ~SoundData() = default;

public:
    /// Copy this object.
    SoundData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == SOUND_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SoundData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SoundData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character smells something

Base operator for all kind of perceptions

*/

class SmellData;
typedef SmartPtr<SmellData> Smell;

static const int SMELL_NO = 37;

/// \brief Character smells something.
///
/** Base operator for all kind of perceptions
 */
class SmellData : public PerceptionData
{
protected:
    /// Construct a SmellData class definition.
    SmellData(SmellData *defaults = NULL) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = SMELL_NO;
    }
    /// Default destructor.
    virtual ~SmellData() = default;

public:
    /// Copy this object.
    SmellData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == SMELL_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<SmellData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(SmellData& data, std::map<std::string, int32_t>& attr_data);
};


/** Character feels something (with fingers usually)

Base operator for all kind of perceptions

*/

class FeelData;
typedef SmartPtr<FeelData> Feel;

static const int FEEL_NO = 38;

/// \brief Character feels something (with fingers usually).
///
/** Base operator for all kind of perceptions
 */
class FeelData : public PerceptionData
{
protected:
    /// Construct a FeelData class definition.
    FeelData(FeelData *defaults = NULL) : 
        PerceptionData((PerceptionData*)defaults)
    {
        m_class_no = FEEL_NO;
    }
    /// Default destructor.
    virtual ~FeelData() = default;

public:
    /// Copy this object.
    FeelData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == FEEL_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<FeelData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(FeelData& data, std::map<std::string, int32_t>& attr_data);
};


/** Something went wrong

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class ErrorData;
typedef SmartPtr<ErrorData> Error;

static const int ERROR_NO = 39;

/// \brief Something went wrong.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class ErrorData : public InfoData
{
protected:
    /// Construct a ErrorData class definition.
    ErrorData(ErrorData *defaults = NULL) : 
        InfoData((InfoData*)defaults)
    {
        m_class_no = ERROR_NO;
    }
    /// Default destructor.
    virtual ~ErrorData() = default;

public:
    /// Copy this object.
    ErrorData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == ERROR_NO) current_class = -1; InfoData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<ErrorData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(ErrorData& data, std::map<std::string, int32_t>& attr_data);
};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_OPERATION_H
