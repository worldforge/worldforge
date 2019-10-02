/*
 Copyright (C) 2019 Erik Ogenvik

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

#ifndef ERIS_USAGE_H
#define ERIS_USAGE_H

#include <string>

namespace Eris {

class Entity;

class View;

struct UsageParameter {
	std::string type;
	std::string constraint;
	int min;
	int max;
};

struct Usage {
	std::string name;
	std::string constraint;
	std::string description;

	std::map<std::string, UsageParameter> params;

	bool operator==(const Usage& x) const {
		return name == x.name;
	}
};

struct TaskUsage {
	std::string name;

	std::map<std::string, UsageParameter> params;

	bool operator==(const TaskUsage& x) const {
		return name == x.name;
	}

};

}

#endif //ERIS_USAGE_H
