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
#pragma once

#include "Filter.h"
#include "ParserDefinitions.h"
#include "Providers_impl.h"
#include "Predicates_impl.h"
#include "ProviderFactory_impl.h"

using namespace boost;
namespace qi = boost::spirit::qi;
using qi::no_case;

namespace EntityFilter {
template<typename EntityT>
Filter<EntityT>::Filter(const std::string& what, const ProviderFactory<EntityT>& factory)
		: m_declaration(what) {
	parser::query_parser<std::string::const_iterator, EntityT> grammar(factory);
	//boost::spirit::qi::debug(grammar.parenthesised_predicate_g);
	auto iter_begin = what.begin();
	auto iter_end = what.end();
	bool parse_success;
	try {
		parse_success = qi::phrase_parse(iter_begin, iter_end, grammar,
										 boost::spirit::qi::space, m_predicate);
	} catch (const std::invalid_argument& e) {
		throw std::invalid_argument(fmt::format("Error when parsing '{}':\n{}", what, e.what()));
	}
	if (!(parse_success && iter_begin == iter_end)) {
		auto parsedPart = what.substr(0, iter_begin - what.begin());
		throw std::invalid_argument(fmt::format("Attempted creating entity filter with invalid query. Query was '{}'.\n Parser error was at '{}'", what, parsedPart));
	}
}

template<typename EntityT>
Filter<EntityT>::~Filter() = default;

template<typename EntityT>
bool Filter<EntityT>::match(const QueryContext<EntityT>& context) const {

	return m_predicate->isMatch(context);
}

template<typename EntityT>
const std::string& Filter<EntityT>::getDeclaration() const {
	return m_declaration;
}

}
