// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifndef COMMON_INHERITANCE_H
#define COMMON_INHERITANCE_H

#include "common/Singleton.h"
#include "common/TypeNode.h"
#include "common/TypeStore.h"

#include "rules/simulation/LocatedEntity.h"
#include "common/AtlasFactories.h"

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <sigc++/signal.h>

#include <memory>

namespace Atlas {
namespace Objects {
class Factories;
}
}

/// \brief Class to manage the inheritance tree for in-game entity types
class Inheritance : public Singleton<Inheritance>, public TypeStore<LocatedEntity> {
protected:
	const Atlas::Objects::Root noClass;

	std::map<std::string, std::unique_ptr<TypeNode<LocatedEntity>>> atlasObjects;

	Atlas::Objects::Factories& m_factories;

public:

	explicit Inheritance(Atlas::Objects::Factories& factories = AtlasFactories::factories);

	~Inheritance() override;

	const std::map<std::string, std::unique_ptr<TypeNode<LocatedEntity>>>& getAllObjects() const {
		return atlasObjects;
	}

	size_t getTypeCount() const override {
		return atlasObjects.size();
	}

	const Atlas::Objects::Root& getClass(const std::string& typeName, Visibility visibility) const;

	int updateClass(const std::string& name,
					const Atlas::Objects::Root& obj);

	const TypeNode<LocatedEntity>* getType(const std::string& typeName) const override;

	bool hasClass(const std::string& parent);

	TypeNode<LocatedEntity>* addChild(const Atlas::Objects::Root& obj) override;

	bool isTypeOf(const std::string& instance,
				  const std::string& base_type) const;

	bool isTypeOf(const TypeNode<LocatedEntity>* instance,
				  const std::string& base_type) const;

	bool isTypeOf(const TypeNode<LocatedEntity>* instance,
				  const TypeNode<LocatedEntity>* base_type) const;

	void flush();

	/**
	 * Emitted when types have been changed.
	 */
	sigc::signal<void(const std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>&)> typesUpdated;

	const Atlas::Objects::Factories& getFactories() const override;

	Atlas::Objects::Factories& getFactories() override;
};


#endif // COMMON_INHERITANCE_H
