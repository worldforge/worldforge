//
// C++ Interface: NumericLesserComparer
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
#ifndef EMBEROGRE_MODEL_MAPPING_CASES_ATTRIBUTECOMPARERSNUMERICLESSERCOMPARER_H
#define EMBEROGRE_MODEL_MAPPING_CASES_ATTRIBUTECOMPARERSNUMERICLESSERCOMPARER_H

#include "NumericComparer.h"
#include "NumericValueHolder.h"

namespace Ember::EntityMapping::Cases::AttributeComparers {

/**
	Test for lesser.
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class NumericLesserComparer : public NumericComparer, private NumericValueHolder {
public:
	explicit NumericLesserComparer(double value);

	/**
	Returns true if the supplied value is lesser than the held value.
	*/
	bool test(double value) override;

};
}







#endif
