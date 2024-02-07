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

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include <string>
#include <memory>

///\brief This class is used to perform matches against an entity.
namespace EntityFilter {
template<typename>
class ProviderFactory;

template<typename>
struct QueryContext;

template<typename>
class Predicate;

template<typename EntityT>
class Filter {
public:
	///\brief Initialize a filter with a given query
	///@param what query to be used for filtering
	///@param factory a factory to handle segments
	Filter(const std::string& what, const ProviderFactory<EntityT>& factory);

	~Filter();

	///\brief test given QueryContext for a match
	bool match(const QueryContext<EntityT>& context) const;

	const std::string& getDeclaration() const;

private:
	const std::string m_declaration;
	//The top predicate node used for testing
	std::shared_ptr<Predicate<EntityT>> m_predicate;
};
}
#endif
