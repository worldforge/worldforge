// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#include "OpRuleHandler.h"

#include "common/debug.h"
#include "common/custom_impl.h"
#include "rules/simulation/Inheritance.h"
#include "common/AtlasFactories.h"

#include <Atlas/Objects/Factories.h>

#include <iostream>
#include <fmt/format.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;


static const bool debug_flag = false;

int OpRuleHandler::installOpDefinition(const std::string& class_name,
									   const std::string& parent,
									   const Atlas::Objects::Root& class_desc,
									   std::string& dependent,
									   std::string& reason) {
	assert(class_name == class_desc->getId());

	Inheritance& i = Inheritance::instance();

	if (!i.hasClass(parent)) {
		cy_debug_print("op_definition \"" << class_name
										  << "\" has non existent parent \"" << parent
										  << "\". Waiting.");
		dependent = parent;
		reason = fmt::format("Operation \"{}\" has parent \"{}\" which does "
							 "not exist.", class_name, parent);
		return 1;
	}

	atlasOpDefinition(class_name, parent);

	if (i.addChild(class_desc) == nullptr) {
		return -1;
	}

	i.getFactories().addFactory(class_name, &Atlas::Objects::generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);

	return 0;
}

int OpRuleHandler::modifyOpDefinition(const std::string& class_name,
									  const Atlas::Objects::Root& class_desc) {
	return 0;
}


int OpRuleHandler::check(const Atlas::Objects::Root& desc) {
	assert(!desc->getParent().empty());
	if (desc->getObjtype() != "op_definition") {
		return -1;
	}
	return 0;
}

int OpRuleHandler::install(const std::string& name,
						   const std::string& parent,
						   const Atlas::Objects::Root& description,
						   std::string& dependent,
						   std::string& reason,
						   std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes) {
	//Ignore changes, as no other factories are affected.
	return installOpDefinition(name, parent, description, dependent, reason);
}

int OpRuleHandler::update(const std::string& name,
						  const Atlas::Objects::Root& desc,
						  std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes) {
	return modifyOpDefinition(name, desc);
}
