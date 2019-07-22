// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011 Erik Ogenvik.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_DISPATCHER_H
#define ATLAS_OBJECTS_DISPATCHER_H


#include <Atlas/Objects/Decoder.h>

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

namespace Atlas { namespace Objects { 

/** Objects hierarchy dispatcher
 *
 * This dispatcher can be bound to a codec, will assemble incoming messages,
 * check their type, convert them into the appropriate instance in the
 * Objects hierarchy and call the relevant member functions, which
 * subclasses can conveniently override to pick up the object.
 *
 * @see Atlas::Objects::Encoder
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Dispatcher : public Atlas::Objects::ObjectsDecoder
{
    typedef void (Dispatcher::*objectArrivedPtr)(const Root&);
    typedef std::map<int, objectArrivedPtr> methodMap_t;
public:
    /// Default destructor.
    virtual ~Dispatcher();

    /// Add a new method for Objects class defined by application
    void addMethod(int, objectArrivedPtr method);
protected:
    /// Store extension methods for Objects classes defined by application
    methodMap_t m_methods;

    /// An unknown object has arrived.
    virtual void unknownObjectArrived(const Root&) { }

    /// An object has arrived for dispatch.
    virtual void objectArrived(const Root&);

    /// call right object*Arrived method
    virtual void dispatchObject(const Root& obj);

    /// Override this to get called when a Root object arrives.
    virtual void objectRootArrived(const Root&) { }
    /// Override this to get called when a RootEntity object arrives.
    virtual void objectRootEntityArrived(const Entity::RootEntity&) { }
    /// Override this to get called when a AdminEntity object arrives.
    virtual void objectAdminEntityArrived(const Entity::AdminEntity&) { }
    /// Override this to get called when a Account object arrives.
    virtual void objectAccountArrived(const Entity::Account&) { }
    /// Override this to get called when a Player object arrives.
    virtual void objectPlayerArrived(const Entity::Player&) { }
    /// Override this to get called when a Admin object arrives.
    virtual void objectAdminArrived(const Entity::Admin&) { }
    /// Override this to get called when a Game object arrives.
    virtual void objectGameArrived(const Entity::Game&) { }
    /// Override this to get called when a GameEntity object arrives.
    virtual void objectGameEntityArrived(const Entity::GameEntity&) { }
    /// Override this to get called when a RootOperation object arrives.
    virtual void objectRootOperationArrived(const Operation::RootOperation&) { }
    /// Override this to get called when a Action object arrives.
    virtual void objectActionArrived(const Operation::Action&) { }
    /// Override this to get called when a Create object arrives.
    virtual void objectCreateArrived(const Operation::Create&) { }
    /// Override this to get called when a Combine object arrives.
    virtual void objectCombineArrived(const Operation::Combine&) { }
    /// Override this to get called when a Divide object arrives.
    virtual void objectDivideArrived(const Operation::Divide&) { }
    /// Override this to get called when a Communicate object arrives.
    virtual void objectCommunicateArrived(const Operation::Communicate&) { }
    /// Override this to get called when a Talk object arrives.
    virtual void objectTalkArrived(const Operation::Talk&) { }
    /// Override this to get called when a Delete object arrives.
    virtual void objectDeleteArrived(const Operation::Delete&) { }
    /// Override this to get called when a Set object arrives.
    virtual void objectSetArrived(const Operation::Set&) { }
    /// Override this to get called when a Affect object arrives.
    virtual void objectAffectArrived(const Operation::Affect&) { }
    /// Override this to get called when a Move object arrives.
    virtual void objectMoveArrived(const Operation::Move&) { }
    /// Override this to get called when a Wield object arrives.
    virtual void objectWieldArrived(const Operation::Wield&) { }
    /// Override this to get called when a Get object arrives.
    virtual void objectGetArrived(const Operation::Get&) { }
    /// Override this to get called when a Perceive object arrives.
    virtual void objectPerceiveArrived(const Operation::Perceive&) { }
    /// Override this to get called when a Look object arrives.
    virtual void objectLookArrived(const Operation::Look&) { }
    /// Override this to get called when a Listen object arrives.
    virtual void objectListenArrived(const Operation::Listen&) { }
    /// Override this to get called when a Sniff object arrives.
    virtual void objectSniffArrived(const Operation::Sniff&) { }
    /// Override this to get called when a Touch object arrives.
    virtual void objectTouchArrived(const Operation::Touch&) { }
    /// Override this to get called when a Login object arrives.
    virtual void objectLoginArrived(const Operation::Login&) { }
    /// Override this to get called when a Logout object arrives.
    virtual void objectLogoutArrived(const Operation::Logout&) { }
    /// Override this to get called when a Imaginary object arrives.
    virtual void objectImaginaryArrived(const Operation::Imaginary&) { }
    /// Override this to get called when a Use object arrives.
    virtual void objectUseArrived(const Operation::Use&) { }
    /// Override this to get called when a Info object arrives.
    virtual void objectInfoArrived(const Operation::Info&) { }
    /// Override this to get called when a Perception object arrives.
    virtual void objectPerceptionArrived(const Operation::Perception&) { }
    /// Override this to get called when a Unseen object arrives.
    virtual void objectUnseenArrived(const Operation::Unseen&) { }
    /// Override this to get called when a Sight object arrives.
    virtual void objectSightArrived(const Operation::Sight&) { }
    /// Override this to get called when a Appearance object arrives.
    virtual void objectAppearanceArrived(const Operation::Appearance&) { }
    /// Override this to get called when a Disappearance object arrives.
    virtual void objectDisappearanceArrived(const Operation::Disappearance&) { }
    /// Override this to get called when a Sound object arrives.
    virtual void objectSoundArrived(const Operation::Sound&) { }
    /// Override this to get called when a Smell object arrives.
    virtual void objectSmellArrived(const Operation::Smell&) { }
    /// Override this to get called when a Feel object arrives.
    virtual void objectFeelArrived(const Operation::Feel&) { }
    /// Override this to get called when a Error object arrives.
    virtual void objectErrorArrived(const Operation::Error&) { }
    /// Override this to get called when a Change object arrives.
    virtual void objectChangeArrived(const Operation::Change&) { }
    /// Override this to get called when a Anonymous object arrives.
    virtual void objectAnonymousArrived(const Entity::Anonymous&) { }
    /// Override this to get called when a Generic object arrives.
    virtual void objectGenericArrived(const Operation::Generic&) { }
};

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_DISPATCHER_H
