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

#ifndef ATLAS_MULTI_LINE_LIST_FORMATTER_H
#define ATLAS_MULTI_LINE_LIST_FORMATTER_H

#include "Formatter.h"

namespace Atlas {

/**
 * @brief A formatter which uses multiple lines for list contents.
 */
class MultiLineListFormatter : public Atlas::Formatter
{
public:
	MultiLineListFormatter(std::ostream& s, Atlas::Bridge& b);

	void mapListItem(std::string name) override;

	void listMapItem() override;
	void listListItem() override;
	void listIntItem(long l) override;
	void listFloatItem(double d) override;
	void listStringItem(std::string s) override;
	void listEnd() override;

};

}
#endif // ATLAS_MULTI_LINE_LIST_FORMATTER_H
