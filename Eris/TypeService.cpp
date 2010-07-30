#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/TypeService.h>

#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Connection.h>
#include <Eris/Exceptions.h>
#include <Eris/Response.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Anonymous.h>

#pragma warning(disable: 4068)  //unknown pragma

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
    defineBuiltin("root", NULL);
 /*   
    defineBuiltin("root_operation", m_types["root"]);
    defineBuiltin("action", m_types["root_operation"]);
    defineBuiltin("get", m_types["action"]);
    defineBuiltin("set", m_types["action"]);
    defineBuiltin("perceive", m_types["get"]);
    defineBuiltin("look", m_types["perceive"]);
    defineBuiltin("login", m_types["get"]);
    
    defineBuiltin("info", m_types["root_operation"]);
    defineBuiltin("error", m_types["info"]);
    defineBuiltin("create", m_types["action"]);
    defineBuiltin("communicate", m_types["create"]);
    defineBuiltin("talk", m_types["communicate"]);
    
    defineBuiltin("perception", m_types["info"]);
    defineBuiltin("sight", m_types["perception"]);
    
    defineBuiltin("root_entity", m_types["root"]);
    defineBuiltin("admin_entity", m_types["root_entity"]);
    defineBuiltin("account", m_types["admin_entity"]);
    defineBuiltin("game_entity", m_types["root_entity"]);
    */
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
	
    // every type already in the map delayed it's sendInfoRequest becuase we weren't inited;
    // go through and fix them now. This allows static construction (or early construction) of
    // things like ClassDispatchers in a moderately controlled fashion.
    for (TypeInfoMap::iterator T=m_types.begin(); T!=m_types.end(); ++T) {
        if (!T->second->isBound()) sendRequest(T->second->getName());
    }
}

TypeInfoPtr TypeService::findTypeByName(const std::string &id)
{
    TypeInfoMap::iterator T = m_types.find(id);
    if (T != m_types.end()) return T->second;
	
    return NULL;
}

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
    TypeInfoMap::iterator T = m_types.find(id);
    if (T != m_types.end()) return T->second;
       
// not found, do some work
    /// @todo Verify the id is not in the authorative invalid ID list
    TypeInfoPtr node = new TypeInfo(id, this);
    m_types[id] = node;
    
    sendRequest(id);
    return node;
}

TypeInfoPtr TypeService::getTypeForAtlas(const Root &obj)
{
    const StringList& parents = obj->getParents();
    
    /* special case code to handle the root object which has no parents. */
    if (parents.empty()) {
        // check that obj->isA(ROOT_NO);
        return getTypeByName("root");
    }

    return getTypeByName(parents.front());
}

#pragma mark -

void TypeService::handleOperation(const RootOperation& op)
{
    if (op->instanceOf(ERROR_NO)) {
        const std::vector<Root>& args(op->getArgs());
        Get request = smart_dynamic_cast<Get>(args[1]);
        if (!request.isValid()) throw InvalidOperation("TypeService got ERROR whose arg is not GET");
        
        recvError(request);
    } else if (op->instanceOf(INFO_NO)) {
        const std::vector<Root>& args(op->getArgs());
        std::string objType = args.front()->getObjtype();
        
        if ((objType == "meta") || 
            (objType == "class") ||
            (objType == "op_definition")) 
        {
            recvTypeInfo(args.front());
        }
    } else {
        error() << "type service got op that wasn't info or error";
    }
}

void TypeService::recvTypeInfo(const Root &atype)
{
    TypeInfoMap::iterator T = m_types.find(atype->getId());
    if (T == m_types.end()) {
        error() << "recived type object with unknown ID " << atype->getId();
        return;
    }
	
    // handle duplicates : this can be caused by waitFors pilling up, for example
    if (T->second->isBound() && (atype->getId() != "root"))
        return;
	
    T->second->processTypeData(atype);
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

    TypeInfoMap::iterator T = m_types.find(request->getId());
    if (T == m_types.end()) {
        // what the fuck? getting out of here...
        throw InvalidOperation("got ERROR(GET()) with request for unknown type: " + request->getId());
    }
    
    warning() << "type " << request->getId() << " undefined on server";
    BadType.emit(T->second);

    delete T->second;
    m_types.erase(T);
}

TypeInfoPtr TypeService::defineBuiltin(const std::string& name, TypeInfo* parent)
{
    assert(m_types.count(name) == 0);
    
    TypeInfo* type = new TypeInfo(name, this);
    m_types[name] = type;
    
    if (parent) type->addParent(parent);
    type->validateBind();
    
    assert(type->isBound());
    return type;
}

} // of namespace Eris
