// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#ifndef COMMON_ROUTER_H
#define COMMON_ROUTER_H

#include "OperationRouter.h"
#include "RouterId.h"

#include <Atlas/Message/Element.h>

#include <string>
#include <map>


class Link;

/**
 * An interface for anything that's identifiable through a RouterId.
 */
struct RouterIdentifiable {
	inline explicit RouterIdentifiable(RouterId id)
			: m_id(id) {
	}

	const RouterId m_id;

	/// \brief Read only accessor for string identity
	std::string getIdAsString() const {
		return m_id.asString();
	}

	/// \brief Read only accessor for Integer identity
	long getIdAsInt() const {
		return m_id.m_intId;
	}
};

struct ExternalRouter {

	/// \brief Process an operation from an external source
	///
	/// The ownership of the operation passed in at this point is handed
	/// over to the router. The calling code must not modify the operation
	/// after passing it to here, or expect the attributes
	/// of the operation to remain the same.
	/// @param op The operation to be processed.
	virtual void externalOperation(const Operation& op, Link&) = 0;
};


/**
 * Used by classes that can be described as RootEntity instances.
 */
struct AtlasDescribable {

	/// \brief Copy the attribute values of this object to an Atlas Entity
	virtual void addToEntity(const Atlas::Objects::Entity::RootEntity&) const = 0;
};

/// \brief This is the base class for any entity which has an Atlas
/// compatible identifier.
///
class Router : public RouterIdentifiable {
public:
	explicit Router(RouterId id);

	Router(const Router&) = delete;

	virtual ~Router();

	static void buildError(const Operation&,
						   const std::string& errstring,
						   const Operation&,
						   const std::string& to);

	/// \brief Report an Error.
	///
	/// The error reported is noted in the log, and an error operation is
	/// generated.
	/// @param op The operation that caused the error.
	/// @param errstring A message describing the error.
	/// @param res The resulting error operation is returned here.
	/// @param to The error operation should be directed to this ID.
	void error(const Operation&, const std::string& errstring, OpVector&,
			   const std::string& to = "") const;

	/// \brief Report an Error to a client.
	///
	/// The error reported generates an error operation.
	/// This is used instead of error() when an event occurs which is of no
	/// interest to the server admin, or world builder, and should only be
	/// be reported to the client. It stops the logs from getting filled
	/// with reports of authentication failures, and other similar occurrences.
	/// @param op The operation that caused the error.
	/// @param errstring A message describing the error.
	/// @param res The resulting error operation is returned here.
	/// @param to The error operation should be directed to this ID.
	void clientError(const Operation&, const std::string& errstring,
					 OpVector&, const std::string& to = "") const;


	/// \brief Dispatch an operation that is to this object
	virtual void operation(const Operation&, OpVector&) = 0;

};

#endif // COMMON_ROUTER_H
