// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011 Erik Ogenvik.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Dispatcher.h>

namespace Atlas { namespace Objects { 

Dispatcher::~Dispatcher()
{
}

void Dispatcher::addMethod(int num, objectArrivedPtr method)
{
    m_methods.insert(std::make_pair(num, method));
}

void Dispatcher::objectArrived(const Root& o)
{
    dispatchObject(o);
}

void Dispatcher::dispatchObject(const Root& obj)
{
    switch(obj->getClassNo()) {
    case ROOT_NO:
        objectRootArrived(smart_dynamic_cast<Root>(obj));
        break;
    case Entity::ROOT_ENTITY_NO:
        objectRootEntityArrived(smart_dynamic_cast<Entity::RootEntity>(obj));
        break;
    case Entity::ADMIN_ENTITY_NO:
        objectAdminEntityArrived(smart_dynamic_cast<Entity::AdminEntity>(obj));
        break;
    case Entity::ACCOUNT_NO:
        objectAccountArrived(smart_dynamic_cast<Entity::Account>(obj));
        break;
    case Entity::PLAYER_NO:
        objectPlayerArrived(smart_dynamic_cast<Entity::Player>(obj));
        break;
    case Entity::ADMIN_NO:
        objectAdminArrived(smart_dynamic_cast<Entity::Admin>(obj));
        break;
    case Entity::GAME_NO:
        objectGameArrived(smart_dynamic_cast<Entity::Game>(obj));
        break;
    case Entity::GAME_ENTITY_NO:
        objectGameEntityArrived(smart_dynamic_cast<Entity::GameEntity>(obj));
        break;
    case Operation::ROOT_OPERATION_NO:
        objectRootOperationArrived(smart_dynamic_cast<Operation::RootOperation>(obj));
        break;
    case Operation::ACTION_NO:
        objectActionArrived(smart_dynamic_cast<Operation::Action>(obj));
        break;
    case Operation::CREATE_NO:
        objectCreateArrived(smart_dynamic_cast<Operation::Create>(obj));
        break;
    case Operation::COMBINE_NO:
        objectCombineArrived(smart_dynamic_cast<Operation::Combine>(obj));
        break;
    case Operation::DIVIDE_NO:
        objectDivideArrived(smart_dynamic_cast<Operation::Divide>(obj));
        break;
    case Operation::COMMUNICATE_NO:
        objectCommunicateArrived(smart_dynamic_cast<Operation::Communicate>(obj));
        break;
    case Operation::TALK_NO:
        objectTalkArrived(smart_dynamic_cast<Operation::Talk>(obj));
        break;
    case Operation::DELETE_NO:
        objectDeleteArrived(smart_dynamic_cast<Operation::Delete>(obj));
        break;
    case Operation::SET_NO:
        objectSetArrived(smart_dynamic_cast<Operation::Set>(obj));
        break;
    case Operation::AFFECT_NO:
        objectAffectArrived(smart_dynamic_cast<Operation::Affect>(obj));
        break;
    case Operation::MOVE_NO:
        objectMoveArrived(smart_dynamic_cast<Operation::Move>(obj));
        break;
    case Operation::WIELD_NO:
        objectWieldArrived(smart_dynamic_cast<Operation::Wield>(obj));
        break;
    case Operation::GET_NO:
        objectGetArrived(smart_dynamic_cast<Operation::Get>(obj));
        break;
    case Operation::PERCEIVE_NO:
        objectPerceiveArrived(smart_dynamic_cast<Operation::Perceive>(obj));
        break;
    case Operation::LOOK_NO:
        objectLookArrived(smart_dynamic_cast<Operation::Look>(obj));
        break;
    case Operation::LISTEN_NO:
        objectListenArrived(smart_dynamic_cast<Operation::Listen>(obj));
        break;
    case Operation::SNIFF_NO:
        objectSniffArrived(smart_dynamic_cast<Operation::Sniff>(obj));
        break;
    case Operation::TOUCH_NO:
        objectTouchArrived(smart_dynamic_cast<Operation::Touch>(obj));
        break;
    case Operation::LOGIN_NO:
        objectLoginArrived(smart_dynamic_cast<Operation::Login>(obj));
        break;
    case Operation::LOGOUT_NO:
        objectLogoutArrived(smart_dynamic_cast<Operation::Logout>(obj));
        break;
    case Operation::IMAGINARY_NO:
        objectImaginaryArrived(smart_dynamic_cast<Operation::Imaginary>(obj));
        break;
    case Operation::USE_NO:
        objectUseArrived(smart_dynamic_cast<Operation::Use>(obj));
        break;
    case Operation::INFO_NO:
        objectInfoArrived(smart_dynamic_cast<Operation::Info>(obj));
        break;
    case Operation::PERCEPTION_NO:
        objectPerceptionArrived(smart_dynamic_cast<Operation::Perception>(obj));
        break;
    case Operation::SIGHT_NO:
        objectSightArrived(smart_dynamic_cast<Operation::Sight>(obj));
        break;
    case Operation::APPEARANCE_NO:
        objectAppearanceArrived(smart_dynamic_cast<Operation::Appearance>(obj));
        break;
    case Operation::DISAPPEARANCE_NO:
        objectDisappearanceArrived(smart_dynamic_cast<Operation::Disappearance>(obj));
        break;
    case Operation::SOUND_NO:
        objectSoundArrived(smart_dynamic_cast<Operation::Sound>(obj));
        break;
    case Operation::SMELL_NO:
        objectSmellArrived(smart_dynamic_cast<Operation::Smell>(obj));
        break;
    case Operation::FEEL_NO:
        objectFeelArrived(smart_dynamic_cast<Operation::Feel>(obj));
        break;
    case Operation::ERROR_NO:
        objectErrorArrived(smart_dynamic_cast<Operation::Error>(obj));
        break;
    case Entity::ANONYMOUS_NO:
        objectAnonymousArrived(smart_dynamic_cast<Entity::Anonymous>(obj));
        break;
    case Operation::GENERIC_NO:
        objectGenericArrived(smart_dynamic_cast<Operation::Generic>(obj));
        break;
    default:
        methodMap_t::const_iterator I = m_methods.find(obj->getClassNo());
        if (I != m_methods.end()) {
            (this->*I->second)(obj);
        } else {
            unknownObjectArrived(obj);
        }
    }
}
} } // namespace Atlas::Objects
