#include <utility>

#include "TypeService.h"

#include "TypeInfo.h"
#include "Log.h"
#include "Connection.h"
#include "Exceptions.h"
#include "Response.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Anonymous.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

TypeService::TypeService(Connection& con) :
		m_con(con),
		m_inited(false) {
	defineBuiltin("root", nullptr);
}

TypeService::~TypeService() = default;

void TypeService::init() {
	assert(!m_inited);
	m_inited = true;

	// every type already in the map delayed it's sendInfoRequest because we weren't inited;
	// go through and fix them now. This allows static construction (or early construction) of
	// things like ClassDispatchers in a moderately controlled fashion.
	for (auto& type: m_types) {
		if (!type.second->isBound()) sendRequest(type.second->getName());
	}
}

TypeInfo* TypeService::findTypeByName(const std::string& id) {
	auto T = m_types.find(id);
	if (T != m_types.end()) {
		return T->second.get();
	}

	return nullptr;
}

TypeInfo* TypeService::getTypeByName(const std::string& id) {
	auto T = m_types.find(id);
	if (T != m_types.end()) {
		return T->second.get();
	}

// not found, do some work
	/// @todo Verify the id is not in the authoritative invalid ID list
	auto node = new TypeInfo(id, *this);
	m_types[id] = std::unique_ptr<TypeInfo>(node);

	sendRequest(id);
	return node;
}

TypeInfo* TypeService::getTypeForAtlas(const Root& obj) {
	/* special case code to handle the root object which has no parents. */
	if (obj->getParent().empty()) {
		// check that obj->isA(ROOT_NO);
		return getTypeByName("root");
	}

	return getTypeByName(obj->getParent());
}

void TypeService::handleOperation(const RootOperation& op) {
	if (op->instanceOf(ERROR_NO)) {
		auto message = getErrorMessage(op);
		logger->trace("Error from server when requesting type: {}", message);
		auto& args = op->getArgs();
		for (const auto& arg: args) {
			Get request = smart_dynamic_cast<Get>(arg);
			if (request) {
				recvError(request);
			}
		}
	} else if (op->instanceOf(CHANGE_NO)) {
		const std::vector<Root>& args(op->getArgs());
		for (const auto& arg: args) {
			if (!arg->isDefaultObjtype()) {
				auto& objType = arg->getObjtype();
				if ((objType == "meta") ||
					(objType == "class") ||
					(objType == "op_definition") ||
					(objType == "archetype")) {
					recvTypeUpdate(arg);
				}
			}
		}
	} else if (op->instanceOf(INFO_NO)) {
		const std::vector<Root>& args(op->getArgs());
		for (const auto& arg: args) {
			if (!arg->isDefaultObjtype()) {
				auto& objType = arg->getObjtype();

				if ((objType == "meta") ||
					(objType == "class") ||
					(objType == "op_definition") ||
					(objType == "archetype")) {
					recvTypeInfo(arg);
				}
			}
		}
	} else {
		logger->error("type service got op that wasn't info or error");
	}
}

void TypeService::recvTypeInfo(const Root& atype) {
	auto T = m_types.find(atype->getId());
	if (T == m_types.end()) {
		logger->error("received type object with unknown ID {}", atype->getId());
		return;
	}

	T->second->processTypeData(atype);
}

void TypeService::recvTypeUpdate(const Root& atype) {
	//A type has been updated on the server. Check if we have it. If so we need to refresh.
	//If we don't have it we should do nothing.
	auto T = m_types.find(atype->getId());
	if (T == m_types.end()) {
		return;
	}

	sendRequest(atype->getId());
}

void TypeService::sendRequest(const std::string& id) {
	// stop premature requests (before the connection is available); when TypeInfo::init
	// is called, the requests will be re-issued manually
	if (!m_inited) return;

	Anonymous what;
	what->setId(id);

	Get get;
	get->setArgs1(what);
	get->setSerialno(getNewSerialno());
	if (!m_type_provider_id.empty()) {
		get->setFrom(m_type_provider_id);
	}

	m_con.getResponder().await(get->getSerialno(), this, &TypeService::handleOperation);
	m_con.send(get);
}

void TypeService::recvError(const Get& get) {
	const std::vector<Root>& args = get->getArgs();

	for (const auto& request: args) {
		if (!request->isDefaultId()) {
			auto T = m_types.find(request->getId());
			if (T == m_types.end()) {
				// This type isn't known, which is strange
				logger->error("got ERROR(GET()) with request for unknown type: {}", request->getId());
				continue;
			}

			logger->warn("type {} undefined on server", request->getId());
			BadType.emit(T->second.get());

			m_types.erase(T);

		}
	}
}

TypeInfo* TypeService::defineBuiltin(const std::string& name, TypeInfo* parent) {
	assert(m_types.count(name) == 0);

	auto type = new TypeInfo(name, *this);
	m_types[name] = std::unique_ptr<TypeInfo>(type);

	if (parent) {
		type->setParent(parent);
	}
	type->validateBind();

	assert(type->isBound());
	return type;
}

void TypeService::setTypeProviderId(std::string id) {
	m_type_provider_id = std::move(id);
}

} // of namespace Eris
