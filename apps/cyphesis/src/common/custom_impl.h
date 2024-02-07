// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2024 Erik Ogenvik
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#pragma once

#include "custom.h"
#include "type_utils.h"
#include "TypeNode_impl.h"
#include "log.h"
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

template<typename EntityT>
void installStandardObjects(TypeStore<EntityT>& i) {
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::RootOperationData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::ActionData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::CreateData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::DeleteData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::InfoData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::SetData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::GetData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::PerceptionData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::ErrorData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::CommunicateData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::MoveData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::AffectData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::PerceiveData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::LoginData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::LogoutData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::SightData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::SoundData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::SmellData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::FeelData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::ImaginaryData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::TalkData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::LookData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::ListenData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::SniffData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::TouchData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::AppearanceData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::DisappearanceData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::UseData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::WieldData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::HitData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::ChangeData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::UnseenData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Operation::ActivityData>());

	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::RootEntityData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::AdminEntityData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::AccountData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::PlayerData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::AdminData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::GameData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::GameEntityData>());
	i.addChild(atlasObjDefinition<Atlas::Objects::Entity::SystemAccountData>());

	//We should now have just as many types registered as there are standard types in Atlas.
	auto typeCount = i.getTypeCount();
	auto anonymousTypeCount = 2; //Anonymous and Generic shouldn't be included
	auto atlasTypeCount = i.getFactories().getKeys().size() - anonymousTypeCount;
	if (typeCount != atlasTypeCount) {
		spdlog::error("There are {} Atlas types, but there are only {} types registered in Cyphesis.", atlasTypeCount, typeCount);
	}


	i.addChild(atlasClass("root_type", "root"));

	// And from here on we need to define the hierarchy as found in the C++
	// base classes. Script classes defined in rulesets need to be added
	// at runtime.
}


template<typename EntityT>
void installCustomOperations(TypeStore<EntityT>& i) {
	auto& atlas_factories = i.getFactories();

	i.addChild(atlasOpDefinition("connect", "set"));
	Atlas::Objects::Operation::CONNECT_NO = atlas_factories.addFactory("connect", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("monitor", "set"));
	Atlas::Objects::Operation::MONITOR_NO = atlas_factories.addFactory("monitor", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("setup", "root_operation"));
	Atlas::Objects::Operation::SETUP_NO = atlas_factories.addFactory("setup", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("tick", "root_operation"));
	Atlas::Objects::Operation::TICK_NO = atlas_factories.addFactory("tick", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("update", "tick"));
	Atlas::Objects::Operation::UPDATE_NO = atlas_factories.addFactory("update", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("thought", "communicate"));
	Atlas::Objects::Operation::THOUGHT_NO = atlas_factories.addFactory("thought", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("goal_info", "communicate"));
	Atlas::Objects::Operation::GOAL_INFO_NO = atlas_factories.addFactory("goal_info", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("teleport", "action"));
	Atlas::Objects::Operation::TELEPORT_NO = atlas_factories.addFactory("teleport", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("commune", "get"));
	Atlas::Objects::Operation::COMMUNE_NO = atlas_factories.addFactory("commune", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("think", "set"));
	Atlas::Objects::Operation::THINK_NO = atlas_factories.addFactory("think", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	//The relay operation is used when an operation needs to be sent from one router to another, and there's no natural way of doing it.
	i.addChild(atlasOpDefinition("relay", "root_operation"));
	Atlas::Objects::Operation::RELAY_NO = atlas_factories.addFactory("relay", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("possess", "set"));
	Atlas::Objects::Operation::POSSESS_NO = atlas_factories.addFactory("possess", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	i.addChild(atlasOpDefinition("close_container", "action"));
	Atlas::Objects::Operation::CLOSE_CONTAINER_NO = atlas_factories.addFactory("close_container", &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);
}

template<typename EntityT>
void installCustomEntities(TypeStore<EntityT>& i) {

	i.addChild(atlasClass("room", "admin_entity"));

	i.addChild(atlasClass("lobby", "admin_entity"));

	i.addChild(atlasClass("server", "admin_entity"));

	i.addChild(atlasClass("task", "admin_entity"));
}

