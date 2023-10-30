/*
 Copyright (C) 2022 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "BindingsEris.h"
#include "RegisterLua.h"

#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeService.h>
#include <Eris/Task.h>
#include <Eris/Metaserver.h>

using namespace Eris;
using namespace Ember::Lua;

void registerBindingsEris(sol::state_view& lua) {
	auto Eris = lua["Eris"].get_or_create<sol::table>();

	registerLua<Avatar>(Eris);
	registerLua<Account>(Eris);
	registerLua<BaseConnection>(Eris);
	registerLua<Entity>(Eris);
	registerLua<Meta>(Eris);
	registerLua<ServerInfo>(Eris);
	registerLua<Task>(Eris);
	registerLua<TypeInfo>(Eris);
	registerLua<TypeService>(Eris);
	registerLua<Usage>(Eris);
	registerLua<View>(Eris);
}