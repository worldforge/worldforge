// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

#include "Script.h"
#include "common/log.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;


template <typename EntityT>
HandlerResult Script<EntityT>::operation(const std::string& opname,
								const Atlas::Objects::Operation::RootOperation& op,
								OpVector& res) {
	return OPERATION_IGNORED;
}

template <typename EntityT>
void Script<EntityT>::hook(const std::string& function, EntityT* entity, OpVector& res) {
}

template <typename EntityT>
void Script<EntityT>::attachPropertyCallbacks(EntityT& entity, std::function<void(const Atlas::Objects::Operation::RootOperation&)> sendWorldFn) {
}


template <typename EntityT>
int Script<EntityT>::getScriptDetails(const Atlas::Message::MapType& script,
							 const std::string& class_name,
							 const std::string& context,
							 std::string& script_package,
							 std::string& script_class) {
	auto J = script.find("name");
	auto Jend = script.end();

	if (J == Jend || !J->second.isString()) {
		spdlog::error("{} \"{}\" script has no name.",
					  context, class_name);
		return -1;
	}
	const std::string& script_name = J->second.String();
	J = script.find("language");
	if (J == Jend || !J->second.isString()) {
		spdlog::error("{} \"{}\" script has no language.",
					  context, class_name);
		return -1;
	}
	const std::string& script_language = J->second.String();
	if (script_language != "python") {
		spdlog::error(R"({} "{}" script has unknown language "{}".)",
					  context, class_name, script_language);
		return -1;
	}
	std::string::size_type ptr = script_name.rfind('.');
	if (ptr == std::string::npos) {
		spdlog::error(R"({} "{}" python script has bad class name "{}".)",
					  context, class_name, script_name);
		return -1;
	}
	script_package = script_name.substr(0, ptr);
	script_class = script_name.substr(ptr + 1);

	return 0;
}
