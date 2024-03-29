/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_SIMPLETYPESTORE_H
#define CYPHESIS_SIMPLETYPESTORE_H

#include "common/TypeStore.h"
#include "rules/ai/MemEntity.h"
#include <memory>

class SimpleTypeStore : public TypeStore<MemEntity> {
public:

	explicit SimpleTypeStore(const PropertyManager<MemEntity>& propertyManager);

	const TypeNode<MemEntity>* getType(const std::string& parent) const override;

	TypeNode<MemEntity>* addChild(const Atlas::Objects::Root& obj) override;

	size_t getTypeCount() const override;

	Atlas::Objects::Factories& getFactories() override;

	const Atlas::Objects::Factories& getFactories() const override;


private:

	const PropertyManager<MemEntity>& m_propertyManager;

	std::map<std::string, std::unique_ptr<TypeNode<MemEntity>>> m_types;

	void readAttributesIntoType(TypeNode<MemEntity>& type, const Atlas::Objects::Root& obj);

};


#endif //CYPHESIS_SIMPLETYPESTORE_H
