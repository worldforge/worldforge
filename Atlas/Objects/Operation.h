// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
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
    virtual ~ActionData();

public:
    /// Copy this object.
    virtual ActionData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ACTION_NO) current_class = -1; RootOperationData::iterate(current_class, attr);}

    //freelist related things
public:
    static ActionData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual ActionData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static ActionData *getDefaultObjectInstance();
private:
    static ActionData *defaults_ActionData;
    static ActionData *begin_ActionData;
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
    virtual ~CreateData();

public:
    /// Copy this object.
    virtual CreateData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == CREATE_NO) current_class = -1; ActionData::iterate(current_class, attr);}

    //freelist related things
public:
    static CreateData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual CreateData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static CreateData *getDefaultObjectInstance();
private:
    static CreateData *defaults_CreateData;
    static CreateData *begin_CreateData;
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
    virtual ~CombineData();

public:
    /// Copy this object.
    virtual CombineData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == COMBINE_NO) current_class = -1; CreateData::iterate(current_class, attr);}

    //freelist related things
public:
    static CombineData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual CombineData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static CombineData *getDefaultObjectInstance();
private:
    static CombineData *defaults_CombineData;
    static CombineData *begin_CombineData;
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
    virtual ~DivideData();

public:
    /// Copy this object.
    virtual DivideData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == DIVIDE_NO) current_class = -1; CreateData::iterate(current_class, attr);}

    //freelist related things
public:
    static DivideData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual DivideData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static DivideData *getDefaultObjectInstance();
private:
    static DivideData *defaults_DivideData;
    static DivideData *begin_DivideData;
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
    virtual ~CommunicateData();

public:
    /// Copy this object.
    virtual CommunicateData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == COMMUNICATE_NO) current_class = -1; CreateData::iterate(current_class, attr);}

    //freelist related things
public:
    static CommunicateData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual CommunicateData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static CommunicateData *getDefaultObjectInstance();
private:
    static CommunicateData *defaults_CommunicateData;
    static CommunicateData *begin_CommunicateData;
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
    virtual ~TalkData();

public:
    /// Copy this object.
    virtual TalkData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == TALK_NO) current_class = -1; CommunicateData::iterate(current_class, attr);}

    //freelist related things
public:
    static TalkData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual TalkData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static TalkData *getDefaultObjectInstance();
private:
    static TalkData *defaults_TalkData;
    static TalkData *begin_TalkData;
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
    virtual ~DeleteData();

public:
    /// Copy this object.
    virtual DeleteData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == DELETE_NO) current_class = -1; ActionData::iterate(current_class, attr);}

    //freelist related things
public:
    static DeleteData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual DeleteData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static DeleteData *getDefaultObjectInstance();
private:
    static DeleteData *defaults_DeleteData;
    static DeleteData *begin_DeleteData;
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
    virtual ~SetData();

public:
    /// Copy this object.
    virtual SetData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == SET_NO) current_class = -1; ActionData::iterate(current_class, attr);}

    //freelist related things
public:
    static SetData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual SetData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static SetData *getDefaultObjectInstance();
private:
    static SetData *defaults_SetData;
    static SetData *begin_SetData;
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
    virtual ~AffectData();

public:
    /// Copy this object.
    virtual AffectData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == AFFECT_NO) current_class = -1; SetData::iterate(current_class, attr);}

    //freelist related things
public:
    static AffectData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual AffectData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static AffectData *getDefaultObjectInstance();
private:
    static AffectData *defaults_AffectData;
    static AffectData *begin_AffectData;
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
    virtual ~MoveData();

public:
    /// Copy this object.
    virtual MoveData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == MOVE_NO) current_class = -1; SetData::iterate(current_class, attr);}

    //freelist related things
public:
    static MoveData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual MoveData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static MoveData *getDefaultObjectInstance();
private:
    static MoveData *defaults_MoveData;
    static MoveData *begin_MoveData;
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
    virtual ~GetData();

public:
    /// Copy this object.
    virtual GetData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == GET_NO) current_class = -1; ActionData::iterate(current_class, attr);}

    //freelist related things
public:
    static GetData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual GetData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static GetData *getDefaultObjectInstance();
private:
    static GetData *defaults_GetData;
    static GetData *begin_GetData;
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
    virtual ~PerceiveData();

public:
    /// Copy this object.
    virtual PerceiveData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == PERCEIVE_NO) current_class = -1; GetData::iterate(current_class, attr);}

    //freelist related things
public:
    static PerceiveData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual PerceiveData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static PerceiveData *getDefaultObjectInstance();
private:
    static PerceiveData *defaults_PerceiveData;
    static PerceiveData *begin_PerceiveData;
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
    virtual ~LookData();

public:
    /// Copy this object.
    virtual LookData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == LOOK_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

    //freelist related things
public:
    static LookData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual LookData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static LookData *getDefaultObjectInstance();
private:
    static LookData *defaults_LookData;
    static LookData *begin_LookData;
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
    virtual ~ListenData();

public:
    /// Copy this object.
    virtual ListenData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == LISTEN_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

    //freelist related things
public:
    static ListenData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual ListenData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static ListenData *getDefaultObjectInstance();
private:
    static ListenData *defaults_ListenData;
    static ListenData *begin_ListenData;
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
    virtual ~SniffData();

public:
    /// Copy this object.
    virtual SniffData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == SNIFF_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

    //freelist related things
public:
    static SniffData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual SniffData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static SniffData *getDefaultObjectInstance();
private:
    static SniffData *defaults_SniffData;
    static SniffData *begin_SniffData;
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
    virtual ~TouchData();

public:
    /// Copy this object.
    virtual TouchData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == TOUCH_NO) current_class = -1; PerceiveData::iterate(current_class, attr);}

    //freelist related things
public:
    static TouchData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual TouchData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static TouchData *getDefaultObjectInstance();
private:
    static TouchData *defaults_TouchData;
    static TouchData *begin_TouchData;
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
    virtual ~LoginData();

public:
    /// Copy this object.
    virtual LoginData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == LOGIN_NO) current_class = -1; GetData::iterate(current_class, attr);}

    //freelist related things
public:
    static LoginData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual LoginData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static LoginData *getDefaultObjectInstance();
private:
    static LoginData *defaults_LoginData;
    static LoginData *begin_LoginData;
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
    virtual ~LogoutData();

public:
    /// Copy this object.
    virtual LogoutData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == LOGOUT_NO) current_class = -1; LoginData::iterate(current_class, attr);}

    //freelist related things
public:
    static LogoutData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual LogoutData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static LogoutData *getDefaultObjectInstance();
private:
    static LogoutData *defaults_LogoutData;
    static LogoutData *begin_LogoutData;
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
    virtual ~ImaginaryData();

public:
    /// Copy this object.
    virtual ImaginaryData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == IMAGINARY_NO) current_class = -1; ActionData::iterate(current_class, attr);}

    //freelist related things
public:
    static ImaginaryData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual ImaginaryData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static ImaginaryData *getDefaultObjectInstance();
private:
    static ImaginaryData *defaults_ImaginaryData;
    static ImaginaryData *begin_ImaginaryData;
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
    virtual ~UseData();

public:
    /// Copy this object.
    virtual UseData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == USE_NO) current_class = -1; ActionData::iterate(current_class, attr);}

    //freelist related things
public:
    static UseData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual UseData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static UseData *getDefaultObjectInstance();
private:
    static UseData *defaults_UseData;
    static UseData *begin_UseData;
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
    virtual ~InfoData();

public:
    /// Copy this object.
    virtual InfoData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == INFO_NO) current_class = -1; RootOperationData::iterate(current_class, attr);}

    //freelist related things
public:
    static InfoData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual InfoData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static InfoData *getDefaultObjectInstance();
private:
    static InfoData *defaults_InfoData;
    static InfoData *begin_InfoData;
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
    virtual ~PerceptionData();

public:
    /// Copy this object.
    virtual PerceptionData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == PERCEPTION_NO) current_class = -1; InfoData::iterate(current_class, attr);}

    //freelist related things
public:
    static PerceptionData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual PerceptionData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static PerceptionData *getDefaultObjectInstance();
private:
    static PerceptionData *defaults_PerceptionData;
    static PerceptionData *begin_PerceptionData;
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
    virtual ~SightData();

public:
    /// Copy this object.
    virtual SightData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == SIGHT_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

    //freelist related things
public:
    static SightData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual SightData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static SightData *getDefaultObjectInstance();
private:
    static SightData *defaults_SightData;
    static SightData *begin_SightData;
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
    virtual ~AppearanceData();

public:
    /// Copy this object.
    virtual AppearanceData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == APPEARANCE_NO) current_class = -1; SightData::iterate(current_class, attr);}

    //freelist related things
public:
    static AppearanceData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual AppearanceData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static AppearanceData *getDefaultObjectInstance();
private:
    static AppearanceData *defaults_AppearanceData;
    static AppearanceData *begin_AppearanceData;
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
    virtual ~DisappearanceData();

public:
    /// Copy this object.
    virtual DisappearanceData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == DISAPPEARANCE_NO) current_class = -1; SightData::iterate(current_class, attr);}

    //freelist related things
public:
    static DisappearanceData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual DisappearanceData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static DisappearanceData *getDefaultObjectInstance();
private:
    static DisappearanceData *defaults_DisappearanceData;
    static DisappearanceData *begin_DisappearanceData;
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
    virtual ~SoundData();

public:
    /// Copy this object.
    virtual SoundData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == SOUND_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

    //freelist related things
public:
    static SoundData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual SoundData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static SoundData *getDefaultObjectInstance();
private:
    static SoundData *defaults_SoundData;
    static SoundData *begin_SoundData;
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
    virtual ~SmellData();

public:
    /// Copy this object.
    virtual SmellData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == SMELL_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

    //freelist related things
public:
    static SmellData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual SmellData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static SmellData *getDefaultObjectInstance();
private:
    static SmellData *defaults_SmellData;
    static SmellData *begin_SmellData;
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
    virtual ~FeelData();

public:
    /// Copy this object.
    virtual FeelData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == FEEL_NO) current_class = -1; PerceptionData::iterate(current_class, attr);}

    //freelist related things
public:
    static FeelData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual FeelData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static FeelData *getDefaultObjectInstance();
private:
    static FeelData *defaults_FeelData;
    static FeelData *begin_FeelData;
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
    virtual ~ErrorData();

public:
    /// Copy this object.
    virtual ErrorData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ERROR_NO) current_class = -1; InfoData::iterate(current_class, attr);}

    //freelist related things
public:
    static ErrorData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual ErrorData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static ErrorData *getDefaultObjectInstance();
private:
    static ErrorData *defaults_ErrorData;
    static ErrorData *begin_ErrorData;
};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_OPERATION_H
