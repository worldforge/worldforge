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


#include "Predicates.h"

#include "common/TypeNode_impl.h"
#include "rules/EntityLocation_impl.h"

#include <algorithm>

namespace EntityFilter {
template<typename EntityT>
ComparePredicate<EntityT>::ComparePredicate(std::shared_ptr<Consumer<QueryContext<EntityT>>> lhs,
											std::shared_ptr<Consumer<QueryContext<EntityT>>> rhs,
											Comparator comparator,
											std::shared_ptr<Consumer<QueryContext<EntityT>>> with) :
		m_lhs(std::move(lhs)),
		m_rhs(std::move(rhs)),
		m_comparator(comparator),
		m_with(std::move(with)) {
	//make sure rhs and lhs exist
	if (!m_lhs) {
		throw std::invalid_argument("Left side statement is invalid");
	}
	if (!m_rhs) {
		throw std::invalid_argument("Right side statement is invalid");
	}
	if (m_comparator == Comparator::INSTANCE_OF) {
		//make sure that left hand side returns an entity and right hand side a typenode
		if (!m_lhs->template isType<const EntityT*>()) {
			throw std::invalid_argument(
					"When using the 'instanceof' comparator, left statement must return an entity. For example, 'entity instance_of types.world'.");
		}
		if (!m_rhs->template isType<const TypeNode<EntityT>*>()) {
			throw std::invalid_argument(
					"When using the 'instanceof' comparator, right statement must return a TypeNode. For example, 'entity instance_of types.world'.");
		}
	} else if (m_comparator == Comparator::CAN_REACH) {
		//make sure that both sides return an entity
		if ((!m_lhs->template isType<const EntityT*>())
			|| ((!m_rhs->template isType<const EntityT*>()) && (!m_rhs->template isType<const QueryEntityLocation<EntityT>*>()))) {
			throw std::invalid_argument(
					"When using the 'can_reach' comparator, both sides must return an entity. For example, 'actor can_reach entity'.");
		}
		if (m_with) {
			if (!m_with->template isType<const EntityT*>()) {
				throw std::invalid_argument(
						"When using the 'can_reach ... with' comparator, all three inputs must return an entity. For example, 'actor can_reach entity with tool'.");
			}
		}
	}

}

template<typename EntityT>
bool ComparePredicate<EntityT>::isMatch(const QueryContext<EntityT>& context) const {
	switch (m_comparator) {
		case Comparator::EQUALS: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			m_rhs->value(right, context);
			return left == right;
		}
		case Comparator::NOT_EQUALS: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			m_rhs->value(right, context);
			return left != right;
		}
		case Comparator::LESS: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (left.isNum()) {
				m_rhs->value(right, context);
				if (right.isNum()) {
					return left.asNum() < right.asNum();
				}
			}
			return false;
		}
		case Comparator::LESS_EQUAL: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (left.isNum()) {
				m_rhs->value(right, context);
				if (right.isNum()) {
					return left.asNum() <= right.asNum();
				}
			}
			return false;
		}
		case Comparator::GREATER: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (left.isNum()) {
				m_rhs->value(right, context);
				if (right.isNum()) {
					return left.asNum() > right.asNum();
				}
			}
			return false;
		}
		case Comparator::GREATER_EQUAL: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (left.isNum()) {
				m_rhs->value(right, context);
				if (right.isNum()) {
					return left.asNum() >= right.asNum();
				}
			}
			return false;
		}
		case Comparator::INSTANCE_OF: {
			//We know that left returns an entity and right returns a type, since we checked in the constructor.
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (left.isPtr()) {
				auto leftType = static_cast<const EntityT*>(left.Ptr())->getType();
				if (leftType) {
					m_rhs->value(right, context);
					if (right.isPtr()) {
						auto rightType = static_cast<const TypeNode<EntityT>*>(right.Ptr());
						if (rightType) {
							return leftType->isTypeOf(rightType);
						}
					}
				}
			}
			return false;
		}
		case Comparator::IN: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (!left.isNone()) {
				m_rhs->value(right, context);
				if (right.isList()) {
					const auto& right_end = right.List().end();
					const auto& right_begin = right.List().begin();
					return std::find(right_begin, right_end, left) != right_end;
				}
			}
			return false;
		}
		case Comparator::INCLUDES: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			if (left.isList()) {
				m_rhs->value(right, context);
				if (!right.isNone()) {
					const auto& left_end = left.List().end();
					const auto& left_begin = left.List().begin();
					return std::find(left_begin, left_end, right) != left_end;
				}
			}
			return false;
		}
		case Comparator::CAN_REACH: {
			Atlas::Message::Element left, right;
			m_lhs->value(left, context);
			auto leftEntity = static_cast<const EntityT*>(left.Ptr());
			m_rhs->value(right, context);

			EntityLocation<EntityT> entityLocation;
			if (m_rhs->template isType<const QueryEntityLocation<EntityT>*>()) {
				auto queryEntityLocation = *static_cast<QueryEntityLocation<EntityT>*>(right.Ptr());
				entityLocation.m_parent = &queryEntityLocation.entity;
				if (queryEntityLocation.pos) {
					entityLocation.m_pos = *queryEntityLocation.pos;
				}
			} else {
				entityLocation.m_parent = static_cast<EntityT*>(right.Ptr());
			}
			float extraReach = 0;
			if (m_with) {
				Atlas::Message::Element with;
				m_with->value(with, context);
				auto withEntity = static_cast<EntityT*>(with.Ptr());
				auto reachProp = withEntity->template getPropertyType<double>("reach");
				if (reachProp) {
					extraReach = (float) reachProp->data();
				}
			}

			if (leftEntity && entityLocation.m_parent) {
				return leftEntity->canReach(entityLocation, extraReach);
			}
			return false;
		}
		default:
			spdlog::error("Could not recognize comparator.");
			return false;
	}
}

template<typename EntityT>
AndPredicate<EntityT>::AndPredicate(std::shared_ptr<Predicate<EntityT>> lhs, std::shared_ptr<Predicate<EntityT>> rhs) :
		m_lhs(std::move(lhs)),
		m_rhs(std::move(rhs)) {
}

template<typename EntityT>
bool AndPredicate<EntityT>::isMatch(const QueryContext<EntityT>& context) const {
	return m_lhs->isMatch(context) && m_rhs->isMatch(context);

}

template<typename EntityT>
OrPredicate<EntityT>::OrPredicate(std::shared_ptr<Predicate<EntityT>> lhs, std::shared_ptr<Predicate<EntityT>> rhs) :
		m_lhs(std::move(lhs)),
		m_rhs(std::move(rhs)) {
}

template<typename EntityT>
bool OrPredicate<EntityT>::isMatch(const QueryContext<EntityT>& context) const {
	return m_lhs->isMatch(context) || m_rhs->isMatch(context);

}

template<typename EntityT>
NotPredicate<EntityT>::NotPredicate(std::shared_ptr<Predicate<EntityT>> pred) :
		m_pred(std::move(pred)) {
}

template<typename EntityT>
bool NotPredicate<EntityT>::isMatch(const QueryContext<EntityT>& context) const {
	return !m_pred->isMatch(context);
}

template<typename EntityT>
BoolPredicate<EntityT>::BoolPredicate(std::shared_ptr<Consumer<QueryContext<EntityT>>> consumer) :
		m_consumer(std::move(consumer)) {
}

template<typename EntityT>
bool BoolPredicate<EntityT>::isMatch(const QueryContext<EntityT>& context) const {
	if (!m_consumer) {
		return false;
	}
	Atlas::Message::Element value;
	m_consumer->value(value, context);
	return value.isInt() && value.Int() != 0;
}

template<typename EntityT>
DescribePredicate<EntityT>::DescribePredicate(std::string description,
											  std::shared_ptr<Predicate<EntityT>> predicate) :
		m_description(std::move(description)),
		m_predicate(std::move(predicate)) {
}

template<typename EntityT>
bool DescribePredicate<EntityT>::isMatch(const QueryContext<EntityT>& context) const {
	bool isMatch = m_predicate->isMatch(context);
	if (!isMatch) {
		if (context.report_error_fn) {
			context.report_error_fn(m_description);
		}
	}
	return isMatch;
}
}
