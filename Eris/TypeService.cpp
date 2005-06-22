#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/TypeService.h>

#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Connection.h>
#include <Eris/TypeBoundRedispatch.h>
#include <Eris/Exceptions.h>
#include <Eris/Response.h>

#include <Atlas/Codecs/XML.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/object_slot.h>

#include <fstream>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

TypeService::TypeService(Connection *con) : 
    m_con(con),
    m_inited(false)
{
    defineBuiltin("root", NULL);
    m_anonymousType = defineBuiltin("anonymous", NULL);
    
    defineBuiltin("root_operation", m_types["root"]);
    defineBuiltin("action", m_types["root_operation"]);
    defineBuiltin("get", m_types["action"]);
    defineBuiltin("set", m_types["action"]);
    defineBuiltin("perceive", m_types["get"]);
    defineBuiltin("look", m_types["perceive"]);
    defineBuiltin("login", m_types["get"]);
    
    defineBuiltin("info", m_types["root_operation"]);
    defineBuiltin("create", m_types["action"]);
    defineBuiltin("communicate", m_types["create"]);
    defineBuiltin("talk", m_types["communicate"]);
    
    defineBuiltin("root_entity", m_types["root"]);
    defineBuiltin("admin_entity", m_types["root_entity"]);
    defineBuiltin("account", m_types["admin_entity"]);
    defineBuiltin("game_entity", m_types["root_entity"]);
}

TypeService::~TypeService()
{
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
        
    Root what;
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
    Root request = args.front();

    TypeInfoMap::iterator T = m_types.find(request->getId());
    if (T == m_types.end()) {
        // what the fuck? getting out of here...
        throw InvalidOperation("got ERROR(GET()) with request for unknown type: " + request->getId());
    }
    
    debug() << "type " << request->getId() << " undefined on server";
    BadType.emit(T->second);

    delete T->second;
    m_types.erase(T);
}

#pragma mark -

TypeService::SpecDecoder::SpecDecoder(TypeService* ts) :
    m_typeService(ts)
{
}
        
void TypeService::SpecDecoder::objectArrived(const Root& def)
{
    m_typeService->registerLocalType(def);
}

void TypeService::readAtlasSpec(const std::string &specfile)
{
    std::fstream specStream(specfile.c_str(), std::ios::in);
    if(!specStream.is_open())
    {
        warning() << "Unable to open Atlas spec file " << specfile
            << ", will obtain all type data from the server";
        return;
    }
 
    debug() << "Found Atlas type data in '" << specfile << "', using for initial type info";
    
    SpecDecoder decode(this);
    Atlas::Codecs::XML specXMLCodec(specStream, decode);
    while (!specStream.eof())
        specXMLCodec.poll(true);
}

void TypeService::registerLocalType(const Root &def)
{
    TypeInfoMap::iterator T = m_types.find(def->getId());
    if (T != m_types.end())
        T->second->processTypeData(def);
    else
        m_types[def->getId()] = new TypeInfo(def, this);
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

bool TypeService::verifyObjectTypes(const Root& obj)
{
    try {
        TypeInfoSet unbound;
        innerVerifyType(obj, unbound);
        
        if (unbound.empty()) return true;
     /*   
        std::string types;
        for (TypeInfoSet::iterator it=unbound.begin(); it!=unbound.end();++it) {
            if (!types.empty()) types.append(", ");
            types.append((*it)->getName());
        }
        debug() << "type verify failed, need [" << types << "]";
     */   
        new TypeBoundRedispatch(m_con, obj, unbound);
    }
    catch (InvalidAtlas& inva) {
        error() << "got malformed atlas: " << inva._msg;
        error() << "bad obj: " << obj;
    }
    
    return false;
}

void TypeService::innerVerifyType(const Root& obj, TypeInfoSet& unbound)
{
    int classNo = obj->getClassNo();
    if (classNo == Atlas::Objects::Entity::ANONYMOUS_NO)
    {
        // if no objtype or parents, it's just plain : we're done
        if (obj->isDefaultObjtype() && obj->isDefaultParents()) return;
        
        // don't try this on type data
        if ((obj->getObjtype() == "class") || (obj->getObjtype() == "op_definition")) return;
        
        TypeInfo* type = getTypeForAtlas(obj);
        if (type == m_anonymousType) {
            warning() << "recieved anonymous object with objtype/parents set: " << obj;
            return;
        }
                
        if (type->isBound()) {
            warning() << type->getName() << " is bound, but got anonymous Object: " << obj;
            return;
        }
        
        unbound.insert(type);
    }

    if (obj->getObjtype() == "op")
        verifyOpArguments(obj, unbound);
}

void TypeService::verifyOpArguments(const Root& obj, TypeInfoSet& unbound)
{
    /*
    Two different cases for verifying op arguments; one where the op
    itself was bound, and hence has the args decoded, and another for
    the case where the op type was unknown, and hence we only have
    Message::Elements. 
    */
    if (obj->instanceOf(Atlas::Objects::Operation::ROOT_OPERATION_NO)) {
        RootOperation op = smart_dynamic_cast<RootOperation>(obj);
        assert(op.isValid());
        const std::vector<Root>& args = op->getArgs();
        
        for  (std::vector<Root>::const_iterator A=args.begin(); A != args.end(); ++A)
            innerVerifyType(*A, unbound);
    } else {
        Atlas::Message::Element args = obj->getAttr("args");
        assert(args.isList());
    
        Atlas::Message::ListType::const_iterator A;
        for  (A = args.asList().begin(); A != args.asList().end(); ++A) {
            Root argObj = Atlas::Objects::Factories::instance()->createObject(A->asMap());
            innerVerifyType(argObj, unbound);
        }
    }
}

} // of namespace Eris
