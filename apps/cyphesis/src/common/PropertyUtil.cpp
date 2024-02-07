/*
 Copyright (C) 2024 Erik Ogenvik

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

#include "PropertyUtil.h"
#include "common/log.h"

std::uint32_t PropertyUtil::flagsForPropertyName(const std::string& name) {
	if (name.size() > 1 && name[0] == '_' && name[1] == '_') {
		return prop_flag_visibility_private;
	} else if (!name.empty() && name[0] == '_') {
		return prop_flag_visibility_protected;
	}
	return 0;
}

bool PropertyUtil::isValidName(const std::string& name) {
	if (name.empty() || name.size() > 32) {
		return false;
	}
	for (auto& character: name) {
		if (std::isalnum(character) || character == '_' || character == '$' || character == '-') {
			continue;
		}
		return false;
	}
	return true;
}

std::pair<ModifierType, std::string> PropertyUtil::parsePropertyModification(const std::string& propertyName) {
	auto pos = propertyName.find('!');
	if (pos != std::string::npos) {
		auto modifier = propertyName.substr(pos + 1);
		auto cleanName = propertyName.substr(0, pos);
		ModifierType type;
		if (modifier == "default") {
			type = ModifierType::Default;
		} else if (modifier == "append") {
			type = ModifierType::Append;
		} else if (modifier == "prepend") {
			type = ModifierType::Prepend;
		} else if (modifier == "subtract") {
			type = ModifierType::Subtract;
		} else if (modifier == "add-fraction") {
			type = ModifierType::AddFraction;
		} else {
			spdlog::warn(R"(Could not recognize "{}" modification in property "{}")", modifier, propertyName);
			return {ModifierType::Default, propertyName};
		}
		return {type, cleanName};
	}
	return {ModifierType::Default, propertyName};
}
