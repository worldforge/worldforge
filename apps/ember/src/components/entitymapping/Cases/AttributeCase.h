//
// C++ Interface: AttributeCase
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef EMBEROGRE_MODEL_MAPPING_CASESATTRIBUTECASE_H
#define EMBEROGRE_MODEL_MAPPING_CASESATTRIBUTECASE_H

#include "Case.h"

#include <Atlas/Message/Element.h>
#include "AttributeComparers/AttributeComparerWrapper.h"

namespace Ember::EntityMapping::Matches {
class AttributeMatch;
}



#include "../Matches/AttributeMatch.h"







namespace Ember::EntityMapping::Cases {


/**
	A child case to an AttributeMatch. This will look for changes to a certain attribute.

	@author Erik Ogenvik <erik@ogenvik.org>
*/
class AttributeCase : public Case<Matches::AttributeMatch> {
public:
	explicit AttributeCase(std::unique_ptr<AttributeComparers::AttributeComparerWrapper> comparerWrapper);

	bool testMatch(const Atlas::Message::Element& attribute);

protected:
	std::unique_ptr<AttributeComparers::AttributeComparerWrapper> mComparerWrapper;
};

}





#endif
