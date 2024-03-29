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

#ifndef CYPHESIS_ACCOUNTPROPERTY_H
#define CYPHESIS_ACCOUNTPROPERTY_H

#include "common/Property.h"
#include "rules/simulation/LocatedEntity.h"

class ServerRouting;

class AccountProperty : public Property<std::string, LocatedEntity> {
public:

	static constexpr const char* property_name = "__account";

	void apply(LocatedEntity&) override;

	AccountProperty* copy() const override;

	static ServerRouting* s_serverRouting;

};


#endif //CYPHESIS_ACCOUNTPROPERTY_H
