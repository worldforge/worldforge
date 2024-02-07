/*
 Copyright (C) 2020 Erik Ogenvik

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

#include "rules/simulation/SpawnProperty.h"
#include "ServerPropertyManager.h"
#include "TeleportProperty.h"
#include "MindProperty.h"
#include "common/PropertyFactory_impl.h"
#include "AccountProperty.h"
#include "rules/simulation/ScriptsProperty.h"

ServerPropertyManager::ServerPropertyManager(Inheritance& inheritance)
: CorePropertyManager(inheritance)
{
    installProperty<TeleportProperty>("linked");
    installProperty<MindProperty>();
    installProperty<AccountProperty>();
    installProperty<SpawnProperty>();

	/**
	 * "__scripts" is meant to be installed on the Type, whereas "__scripts_instance" is meant to be installed on the entity instance.
	 * This way it should be possible to both override type scripts, and to alternatively add new ones to specific instances.
	 */
	installProperty<ScriptsProperty>("__scripts");
	installProperty<ScriptsProperty>("__scripts_instance");

}

ServerPropertyManager::~ServerPropertyManager() = default;
