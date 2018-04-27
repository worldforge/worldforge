#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "TypeService.h"

#include "TypeInfo.h"
#include "Log.h"
#include "Connection.h"
#include "Exceptions.h"
#include "Response.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Anonymous.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

TypeService::TypeService(Connection *con) : 
    m_con(con),
    m_inited(false)
{
    defineBuiltin("root", nullptr);
}

TypeService::~TypeService()
{
    for (TypeInfoMap::const_iterator I = m_types.begin(); I != m_types.end(); ++I) {
        delete I->second;
    }
}

void TypeService::init()
{
    assert(!m_inited);
    m_inited = true;
	
    // every type already in the map delayed it's sendInfoRequest because we weren't inited;
    // go through and fix them now. This allows static construction (or early construction) of
    // things like ClassDispatchers in a moderately controlled fashion.
    for (auto& type : m_types) {
        if (!type.second->isBound()) sendRequest(type.second->getName());
    }
}

TypeInfoPtr TypeService::findTypeByName(const std::string &id)
{
	auto T = m_types.find(id);
    if (T != m_types.end()) return T->second;
	
    return nullptr;
}

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
	auto T = m_types.find(id);
    if (T != m_types.end()) return T->second;
       
// not found, do some work
    /// @todo Verify the id is not in the authoritative invalid ID list
    auto node = new TypeInfo(id, this);
    m_types[id] = node;
    
    sendRequest(id);
    return node;
}

TypeInfoPtr TypeService::getTypeForAtlas(const Root &obj)
{
    /* special case code to handle the root object which has no parents. */
    if (obj->getParent().empty()) {
        // check that obj->isA(ROOT_NO);
        return getTypeByName("root");
    }

    return getTypeByName(obj->getParent());
}

void TypeService::handleOperation(const RootOperation& op)
{
    if (op->instanceOf(ERROR_NO)) {
    	auto message = getErrorMessage(op);
    	notice() << "Error from server when requesting type: " << message;
        auto& args = op->getArgs();
        //First arg is the error message
        if (args.size() > 1) {
            Get request = smart_dynamic_cast<Get>(args[1]);
            if (request) {
				recvError(request);
			}
        }
    } else if (op->instanceOf(INFO_NO)) {
        const std::vector<Root>& args(op->getArgs());
        if (!args.empty()) {
            auto& objType = args.front()->getObjtype();

            if ((objType == "meta") ||
                (objType == "class") ||
                (objType == "op_definition") ||
                (objType == "archetype")) {
                recvTypeInfo(args.front());
            }
        }
    } else if (op->getParent() == "change") {
        const std::vector<Root>& args(op->getArgs());
        if (!args.empty()) {
            auto& objType = args.front()->getObjtype();
            if ((objType == "meta") ||
                (objType == "class") ||
                (objType == "op_definition") ||
                (objType == "archetype")) {
                recvTypeUpdate(args.front());
            }
        }
    } else {
        error() << "type service got op that wasn't info or error";
    }
}

void TypeService::recvTypeInfo(const Root &atype)
{
	auto T = m_types.find(atype->getId());
    if (T == m_types.end()) {
        error() << "received type object with unknown ID " << atype->getId();
        return;
    }
	
    T->second->processTypeData(atype);
}

void TypeService::recvTypeUpdate(const Root &atype)
{
    //A type has been updated on the server. Check if we have it. If so we need to refresh.
    //If we don't have it we should do nothing.
    auto T = m_types.find(atype->getId());
    if (T == m_types.end()) {
        return;
    }

    sendRequest(atype->getId());
}

void TypeService::sendRequest(const std::string &id)
{
    // stop premature requests (before the connection is available); when TypeInfo::init
    // is called, the requests will be re-issued manually
    if (!m_inited) return;
        
    Anonymous what;
    what->setId(id);
    
    Get get;
    get->setArgs1(what);
    get->setSerialno(getNewSerialno());
    
    m_con->getResponder()->await(get->getSerialno(), this, &TypeService::handleOperation);
    m_con->send(get);
}

void TypeService::recvError(const Get& get)
{
    const std::vector<Root>& args = get->getArgs();
    const Root & request = args.front();

	auto T = m_types.find(request->getId());
    if (T == m_types.end()) {
        // This type isn't known, which is strange
		error() << "got ERROR(GET()) with request for unknown type: " + request->getId();
		return;
    }
    
    warning() << "type " << request->getId() << " undefined on server";
    BadType.emit(T->second);

    delete T->second;
    m_types.erase(T);
}

TypeInfoPtr TypeService::defineBuiltin(const std::string& name, TypeInfo* parent)
{
    assert(m_types.count(name) == 0);

    auto type = new TypeInfo(name, this);
    m_types[name] = type;
    
    if (parent) {
        type->setParent(parent);
    }
    type->validateBind();
    
    assert(type->isBound());
    return type;
}

} // of namespace Eris
