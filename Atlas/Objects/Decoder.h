// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_DECODER_H
#define ATLAS_OBJECTS_DECODER_H

#include "../Message/DecoderBase.h"

#include "Root.h"
#include "Entity/Account.h"
#include "Entity/Admin.h"
#include "Entity/AdminEntity.h"
#include "Entity/Game.h"
#include "Entity/Player.h"
#include "Entity/RootEntity.h"
#include "Operation/Action.h"
#include "Operation/Appearance.h"
#include "Operation/Combine.h"
#include "Operation/Communicate.h"
#include "Operation/Create.h"
#include "Operation/Delete.h"
#include "Operation/Disappearance.h"
#include "Operation/Divide.h"
#include "Operation/Error.h"
#include "Operation/Feel.h"
#include "Operation/Get.h"
#include "Operation/Imaginary.h"
#include "Operation/Info.h"
#include "Operation/Listen.h"
#include "Operation/Login.h"
#include "Operation/Logout.h"
#include "Operation/Look.h"
#include "Operation/Move.h"
#include "Operation/Perceive.h"
#include "Operation/Perception.h"
#include "Operation/RootOperation.h"
#include "Operation/Set.h"
#include "Operation/Sight.h"
#include "Operation/Smell.h"
#include "Operation/Sniff.h"
#include "Operation/Sound.h"
#include "Operation/Talk.h"
#include "Operation/Touch.h"

namespace Atlas { namespace Objects {

class Decoder : public Atlas::Message::DecoderBase
{
protected:
    virtual void ObjectArrived(const Atlas::Message::Object&);

    virtual void UnknownObjectArrived(const Atlas::Message::Object&) = 0;

    virtual void ObjectArrived(const Root&) = 0;

    virtual void ObjectArrived(const Entity::Account&) = 0;
    virtual void ObjectArrived(const Entity::Admin&) = 0;
    virtual void ObjectArrived(const Entity::AdminEntity&) = 0;
    virtual void ObjectArrived(const Entity::Game&) = 0;
    virtual void ObjectArrived(const Entity::Player&) = 0;
    virtual void ObjectArrived(const Entity::RootEntity&) = 0;

    virtual void ObjectArrived(const Operation::Action&) = 0;
    virtual void ObjectArrived(const Operation::Appearance&) = 0;
    virtual void ObjectArrived(const Operation::Combine&) = 0;
    virtual void ObjectArrived(const Operation::Communicate&) = 0;
    virtual void ObjectArrived(const Operation::Create&) = 0;
    virtual void ObjectArrived(const Operation::Delete&) = 0;
    virtual void ObjectArrived(const Operation::Disappearance&) = 0;
    virtual void ObjectArrived(const Operation::Divide&) = 0;
    virtual void ObjectArrived(const Operation::Error&) = 0;
    virtual void ObjectArrived(const Operation::Feel&) = 0;
    virtual void ObjectArrived(const Operation::Get&) = 0;
    virtual void ObjectArrived(const Operation::Imaginary&) = 0;
    virtual void ObjectArrived(const Operation::Info&) = 0;
    virtual void ObjectArrived(const Operation::Listen&) = 0;
    virtual void ObjectArrived(const Operation::Login&) = 0;
    virtual void ObjectArrived(const Operation::Logout&) = 0;
    virtual void ObjectArrived(const Operation::Look&) = 0;
    virtual void ObjectArrived(const Operation::Move&) = 0;
    virtual void ObjectArrived(const Operation::Perceive&) = 0;
    virtual void ObjectArrived(const Operation::Perception&) = 0;
    virtual void ObjectArrived(const Operation::RootOperation&) = 0;
    virtual void ObjectArrived(const Operation::Set&) = 0;
    virtual void ObjectArrived(const Operation::Sight&) = 0;
    virtual void ObjectArrived(const Operation::Smell&) = 0;
    virtual void ObjectArrived(const Operation::Sniff&) = 0;
    virtual void ObjectArrived(const Operation::Sound&) = 0;
    virtual void ObjectArrived(const Operation::Talk&) = 0;
    virtual void ObjectArrived(const Operation::Touch&) = 0;
    
};

} } // namespace Atlas::Objects
#endif
