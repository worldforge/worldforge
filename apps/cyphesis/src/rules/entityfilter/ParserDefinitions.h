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

#ifndef RULESETS_FILTER_PARSERDEFINITIONS_H_
#define RULESETS_FILTER_PARSERDEFINITIONS_H_

#include "Filter.h"
#include "Providers.h"
#include "Predicates.h"
#include "ProviderFactory.h"

#include <boost/spirit/include/qi.hpp>

namespace EntityFilter::parser {

namespace qi = boost::spirit::qi;

//A symbol table for comparators. Matches char and synthesises Comparator attribute
template<typename EntityT>
struct comparators_ : qi::symbols<char, typename ComparePredicate<EntityT>::Comparator> {

	comparators_() {
		this->add
				("=", ComparePredicate<EntityT>::Comparator::EQUALS)
				("!=", ComparePredicate<EntityT>::Comparator::NOT_EQUALS)
				(">", ComparePredicate<EntityT>::Comparator::GREATER)
				(">=", ComparePredicate<EntityT>::Comparator::GREATER_EQUAL)
				("<", ComparePredicate<EntityT>::Comparator::LESS)
				("<=", ComparePredicate<EntityT>::Comparator::LESS_EQUAL)
				("instance_of", ComparePredicate<EntityT>::Comparator::INSTANCE_OF)
				("in", ComparePredicate<EntityT>::Comparator::IN)
				("includes", ComparePredicate<EntityT>::Comparator::INCLUDES)
				("can_reach", ComparePredicate<EntityT>::Comparator::CAN_REACH);
	}

};

template<typename Iterator, typename EntityT>
struct query_parser : qi::grammar<Iterator, std::shared_ptr<Predicate<EntityT>>(),
		qi::space_type, qi::locals<std::shared_ptr<Predicate<EntityT>>>> {
	explicit query_parser(const ProviderFactory<EntityT>& factory);

	const ProviderFactory<EntityT>& m_factory;
	qi::rule<Iterator, std::string()> comp_operator_g;
	qi::rule<Iterator, std::string()> logical_operator_g;
	qi::rule<Iterator, std::string(), qi::space_type> segment_attribute_g;
	qi::rule<Iterator, EntityFilter::Segment(), qi::space_type> segment_g;
	qi::rule<Iterator, EntityFilter::Segment(), qi::space_type> special_segment_g;
	qi::rule<Iterator, std::string(), qi::space_type, qi::locals<char> > quoted_string_g;
	qi::rule<Iterator, SegmentsList(), qi::space_type> segmented_expr_g;
	qi::rule<Iterator, SegmentsList(), qi::space_type> delimited_segmented_expr_g;
	qi::rule<Iterator, std::shared_ptr<Consumer<QueryContext<EntityT>>>(), qi::locals<std::vector<Atlas::Message::Element>, bool>, qi::space_type> consumer_g;
	qi::rule<Iterator, std::shared_ptr<Predicate<EntityT>>(), qi::space_type, qi::locals<std::shared_ptr<Predicate<EntityT>>, std::shared_ptr<Consumer<QueryContext<EntityT>>>, std::shared_ptr<Predicate<EntityT>>, typename ComparePredicate<EntityT>::Comparator>> comparer_predicate_g;
	qi::rule<Iterator, std::shared_ptr<Predicate<EntityT>>(), qi::space_type, qi::locals<std::shared_ptr<Predicate<EntityT>>>> predicate_g;
	qi::rule<Iterator, std::shared_ptr<Predicate<EntityT>>(), qi::space_type, qi::locals<std::shared_ptr<Predicate<EntityT>>>> parenthesised_predicate_g;

	//An instance of comparators symbol table
	comparators_<EntityT> comparators;
};

}

#endif
