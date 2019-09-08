// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2019 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { 

int Factories::enumMax = 43;

void Factories::installStandardTypes()
{

    addFactory<RootData>("root", ROOT_NO);

    addFactory<Entity::RootEntityData>("root_entity", Entity::ROOT_ENTITY_NO);

    addFactory<Entity::AdminEntityData>("admin_entity", Entity::ADMIN_ENTITY_NO);

    addFactory<Entity::AccountData>("account", Entity::ACCOUNT_NO);

    addFactory<Entity::PlayerData>("player", Entity::PLAYER_NO);

    addFactory<Entity::AdminData>("admin", Entity::ADMIN_NO);

    addFactory<Entity::GameData>("game", Entity::GAME_NO);

    addFactory<Entity::GameEntityData>("game_entity", Entity::GAME_ENTITY_NO);

    addFactory<Operation::RootOperationData>("root_operation", Operation::ROOT_OPERATION_NO);

    addFactory<Operation::ActionData>("action", Operation::ACTION_NO);

    addFactory<Operation::CreateData>("create", Operation::CREATE_NO);

    addFactory<Operation::CommunicateData>("communicate", Operation::COMMUNICATE_NO);

    addFactory<Operation::TalkData>("talk", Operation::TALK_NO);

    addFactory<Operation::DeleteData>("delete", Operation::DELETE_NO);

    addFactory<Operation::SetData>("set", Operation::SET_NO);

    addFactory<Operation::AffectData>("affect", Operation::AFFECT_NO);

    addFactory<Operation::HitData>("hit", Operation::HIT_NO);

    addFactory<Operation::MoveData>("move", Operation::MOVE_NO);

    addFactory<Operation::WieldData>("wield", Operation::WIELD_NO);

    addFactory<Operation::GetData>("get", Operation::GET_NO);

    addFactory<Operation::PerceiveData>("perceive", Operation::PERCEIVE_NO);

    addFactory<Operation::LookData>("look", Operation::LOOK_NO);

    addFactory<Operation::ListenData>("listen", Operation::LISTEN_NO);

    addFactory<Operation::SniffData>("sniff", Operation::SNIFF_NO);

    addFactory<Operation::TouchData>("touch", Operation::TOUCH_NO);

    addFactory<Operation::LoginData>("login", Operation::LOGIN_NO);

    addFactory<Operation::LogoutData>("logout", Operation::LOGOUT_NO);

    addFactory<Operation::ImaginaryData>("imaginary", Operation::IMAGINARY_NO);

    addFactory<Operation::UseData>("use", Operation::USE_NO);

    addFactory<Operation::InfoData>("info", Operation::INFO_NO);

    addFactory<Operation::PerceptionData>("perception", Operation::PERCEPTION_NO);

    addFactory<Operation::UnseenData>("unseen", Operation::UNSEEN_NO);

    addFactory<Operation::SightData>("sight", Operation::SIGHT_NO);

    addFactory<Operation::AppearanceData>("appearance", Operation::APPEARANCE_NO);

    addFactory<Operation::DisappearanceData>("disappearance", Operation::DISAPPEARANCE_NO);

    addFactory<Operation::SoundData>("sound", Operation::SOUND_NO);

    addFactory<Operation::SmellData>("smell", Operation::SMELL_NO);

    addFactory<Operation::FeelData>("feel", Operation::FEEL_NO);

    addFactory<Operation::ErrorData>("error", Operation::ERROR_NO);

    addFactory<Operation::ChangeData>("change", Operation::CHANGE_NO);

    addFactory<Entity::AnonymousData>("anonymous", Entity::ANONYMOUS_NO);

    addFactory<Operation::GenericData>("generic", Operation::GENERIC_NO);
}
} } // namespace Atlas::Objects
