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

#ifndef PREDICATES_H_
#define PREDICATES_H_

#include "Providers.h"

#include <memory>
#include <string>

namespace EntityFilter {

template<typename>
struct QueryContext;

template<typename>
class Consumer;

template<typename EntityT>
class Predicate {
public:
	virtual ~Predicate() = default;

	virtual bool isMatch(const QueryContext<EntityT>& context) const = 0;
};


template<typename EntityT>
class ComparePredicate : public Predicate<EntityT> {
public:

	enum class Comparator {
		EQUALS, NOT_EQUALS, INSTANCE_OF, IN, INCLUDES, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, CAN_REACH
	};

	ComparePredicate(std::shared_ptr<Consumer<QueryContext<EntityT>>> lhs,
					 std::shared_ptr<Consumer<QueryContext<EntityT>>> rhs,
					 Comparator comparator,
					 std::shared_ptr<Consumer<QueryContext<EntityT>>> with = nullptr);

	bool isMatch(const QueryContext<EntityT>& context) const override;

	std::shared_ptr<Consumer<QueryContext<EntityT>>> m_lhs;
	std::shared_ptr<Consumer<QueryContext<EntityT>>> m_rhs;
	const Comparator m_comparator;
	std::shared_ptr<Consumer<QueryContext<EntityT>>> m_with;
};

template<typename EntityT>
class DescribePredicate : public Predicate<EntityT> {
public:

	DescribePredicate(std::string description,
					  std::shared_ptr<Predicate<EntityT>> predicate);

	bool isMatch(const QueryContext<EntityT>& context) const override;

	std::string m_description;
	std::shared_ptr<Predicate<EntityT>> m_predicate;
};

template<typename EntityT>
class AndPredicate : public Predicate<EntityT> {
public:
	AndPredicate(std::shared_ptr<Predicate<EntityT>> lhs, std::shared_ptr<Predicate<EntityT>> rhs);

	bool isMatch(const QueryContext<EntityT>& context) const override;

	std::shared_ptr<Predicate<EntityT>> m_lhs;
	std::shared_ptr<Predicate<EntityT>> m_rhs;
};

template<typename EntityT>
class OrPredicate : public Predicate<EntityT> {
public:
	OrPredicate(std::shared_ptr<Predicate<EntityT>> lhs, std::shared_ptr<Predicate<EntityT>> rhs);

	bool isMatch(const QueryContext<EntityT>& context) const override;

	std::shared_ptr<Predicate<EntityT>> m_lhs;
	std::shared_ptr<Predicate<EntityT>> m_rhs;
};

template<typename EntityT>
class NotPredicate : public Predicate<EntityT> {
public:
	explicit NotPredicate(std::shared_ptr<Predicate<EntityT>> pred);

	bool isMatch(const QueryContext<EntityT>& context) const override;

	const std::shared_ptr<Predicate<EntityT>> m_pred;
};

template<typename EntityT>
class BoolPredicate : public Predicate<EntityT> {
public:
	explicit BoolPredicate(std::shared_ptr<Consumer<QueryContext<EntityT>>> consumer);

	bool isMatch(const QueryContext<EntityT>& context) const override;

	const std::shared_ptr<Consumer<QueryContext<EntityT>>> m_consumer;
};

}

#endif
