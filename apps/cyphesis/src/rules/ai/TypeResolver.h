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

#ifndef CYPHESIS_TYPERESOLVER_H
#define CYPHESIS_TYPERESOLVER_H

#include "common/OperationRouter.h"
#include "common/TypeStore.h"
#include "MemEntity.h"

#include <map>
#include <string>
#include <set>
#include <Atlas/Objects/RootEntity.h>
#include <optional>


class TypeResolver {
public:

	explicit TypeResolver(TypeStore<MemEntity>& typeStore);

	std::optional<RouterId> m_typeProviderId;

	std::set<const TypeNode<MemEntity>*> InfoOperation(const Operation& op, OpVector& res);

	const TypeNode<MemEntity>* requestType(const std::string& id, OpVector& res);

	const TypeStore<MemEntity>& getTypeStore() const;

private:

	TypeStore<MemEntity>& m_typeStore;

	struct PendingType {
		Atlas::Objects::Root ent;
		std::set<std::string> childTypes;
	};

	std::map<std::string, PendingType> m_pendingTypes;

	std::set<const TypeNode<MemEntity>*> processTypeData(const Atlas::Objects::Root& data, OpVector& res);


	std::set<const TypeNode<MemEntity>*> resolveType(const std::string& id, const Atlas::Objects::Root& ent, OpVector& res);
};


#endif //CYPHESIS_TYPERESOLVER_H
