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
public:
    virtual ~Decoder();

protected:
    virtual void ObjectArrived(const Atlas::Message::Object&);

    virtual void UnknownObjectArrived(const Atlas::Message::Object&) { }

    virtual void ObjectArrived(const Root&) { }

    virtual void ObjectArrived(const Entity::Account&) { }
    virtual void ObjectArrived(const Entity::Admin&) { }
    virtual void ObjectArrived(const Entity::AdminEntity&) { }
    virtual void ObjectArrived(const Entity::Game&) { }
    virtual void ObjectArrived(const Entity::Player&) { }
    virtual void ObjectArrived(const Entity::RootEntity&) { }

    virtual void ObjectArrived(const Operation::Action&) { }
    virtual void ObjectArrived(const Operation::Appearance&) { }
    virtual void ObjectArrived(const Operation::Combine&) { }
    virtual void ObjectArrived(const Operation::Communicate&) { }
    virtual void ObjectArrived(const Operation::Create&) { }
    virtual void ObjectArrived(const Operation::Delete&) { }
    virtual void ObjectArrived(const Operation::Disappearance&) { }
    virtual void ObjectArrived(const Operation::Divide&) { }
    virtual void ObjectArrived(const Operation::Error&) { }
    virtual void ObjectArrived(const Operation::Feel&) { }
    virtual void ObjectArrived(const Operation::Get&) { }
    virtual void ObjectArrived(const Operation::Imaginary&) { }
    virtual void ObjectArrived(const Operation::Info&) { }
    virtual void ObjectArrived(const Operation::Listen&) { }
    virtual void ObjectArrived(const Operation::Login&) { }
    virtual void ObjectArrived(const Operation::Logout&) { }
    virtual void ObjectArrived(const Operation::Look&) { }
    virtual void ObjectArrived(const Operation::Move&) { }
    virtual void ObjectArrived(const Operation::Perceive&) { }
    virtual void ObjectArrived(const Operation::Perception&) { }
    virtual void ObjectArrived(const Operation::RootOperation&) { }
    virtual void ObjectArrived(const Operation::Set&) { }
    virtual void ObjectArrived(const Operation::Sight&) { }
    virtual void ObjectArrived(const Operation::Smell&) { }
    virtual void ObjectArrived(const Operation::Sniff&) { }
    virtual void ObjectArrived(const Operation::Sound&) { }
    virtual void ObjectArrived(const Operation::Talk&) { }
    virtual void ObjectArrived(const Operation::Touch&) { }
    
};

} } // namespace Atlas::Objects
#endif
