#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/typeService.h>

#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Connection.h>
#include <Eris/Utils.h>
#include <Eris/redispatch.h>
#include <Eris/Exceptions.h>

#include <Atlas/Codecs/XML.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

#include <sigc++/object_slot.h>

#include <fstream>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;

namespace Eris
{

TypeService::TypeService(Connection *con) : 
    m_con(con)
{
    /* this block here provides the foundation objects locally, no matter what
    the server does. this reduces some initial traffic during login and
    startup. */
    registerLocalType(Root::Class());
    registerLocalType(Atlas::Objects::Entity::RootEntity::Class());
    registerLocalType(RootOperation::Class());
    registerLocalType(Get::Class());
    registerLocalType(Info::Class());
    registerLocalType(Error::Class());

    // try to read atlas.xml to boot-strap stuff faster
    readAtlasSpec("atlas.xml");
}

void TypeService::init()
{
    // build the root node, install into the global map and kick off the GET
    getTypeByName("root");
	
    // every type already in the map delayed it's sendInfoRequest becuase we weren't inited;
    // go through and fix them now. This allows static construction (or early construction) of
    // things like ClassDispatchers in a moderately controlled fashion.
    for (TypeInfoMap::iterator T=m_types.begin(); T!=m_types.end(); ++T)
        sendRequest(T->second->getName());
}

TypeInfoPtr TypeService::findTypeByName(const std::string &id)
{
    TypeInfoMap::iterator T = m_types.find(id);
    if (T != m_types.end())
        return T->second;
	
    return NULL;
}

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
    TypeInfoMap::iterator T = m_types.find(id);
    if (T != m_types.end())
        return T->second;
    
// not found, do some work
    debug() << "Requesting type data for " << id;
    
    /// @todo Verify the id is not in the authorative invalid ID list
    TypeInfoPtr node = new TypeInfo(id, this);
    m_types[id] = node;
    
    sendRequest(id);
    return node;
}

TypeInfoPtr TypeService::getTypeForAtlas(const Root &obj)
{
    const std::list<std::string>& parents = obj->getParents();
    
    /* special case code to handle the root object which has no parents. */
    if (parents.empty()) {
        // check that obj->isA(ROOT_NO);
        return getTypeByName("root");
    }

    return getTypeByName(parents.front());
}

#pragma mark -

class TypeBoundRedispatch : public Redispatch
{
public:
    TypeBoundRedispatch(Connection* con, const Root& obj) :
        Redispatch(con, obj)
    { ; }
    
    void onBound()
    {
        post();
    }
};

RouterResult TypeService::redispatchWhenBound(TypeInfoPtr ty, const Root& obj)
{
    if (ty->isBound())
    {
        error() << "called redispatchWhenBound for bound type " << ty->getName();
        // try and do the right thing
        m_con->postForDispatch(obj);
        return WILL_REDISPATCH;
    }

    TypeBoundRedispatch* tbr = new TypeBoundRedispatch(m_con, obj);
    ty->Bound.connect(SigC::slot(*tbr, TypeBoundRedispatch::onBound));
 
    // and stop processing the op for now   
    return WILL_REDISPATCH;
}

#pragma mark -

RouterResult TypeService::handleOperation(const RootOperation& op)
{
    // skip any operation who's refno doesn't match the serial of a 
    // request we made
    if (m_typeRequests.find(op->getRefno()) == m_typeRequests.end())
        return IGNORED;
        
    if (op->isA(INFO_NO))
    {
        const std::vector<Root>& args(op->getArgs());
        std::string objType = args.front()->getObjtype();
        
        if ((objType == "meta") || (objType == "class") || (objType == "op_definition"))
        {
            recvTypeInfo(args.front());
            m_typeRequests.erase(op->getRefno());
            return HANDLED;
        }
    }
    
    if (op->isA(ERROR_NO))
    {
        const std::vector<Root>& args(op->getArgs());
        Get request = smart_dynamic_cast<Get>(args.front());
        if (!request)
            throw InvalidOperation("TypeService got ERROR whose arg is not GET");
            
        assert(m_typeRequests.count(request->getSerialno()) == 1);
        m_typeRequests.erase((request->getSerialno());
        
        recvError(request);
        return Router::HANDLED;
    }
    
    error() << "type service got op that wasn't get or error";
    return IGNORED;
}

void TypeService::recvTypeInfo(const Root &atype)
{
    std::string id = atype.getId();
    TypeInfoMap::iterator T = m_types.find(id);
    if (T == m_types.end())
    {
        throw IllegalObject(atype, "type object's ID (" + id + ") is unknown");
    }
	
    // handle duplicates : this can be caused by waitFors pilling up, for example
    if (T->second->isBound() && (id!="root"))
        return;
	
    T->second->processTypeData(atype);
}

void TypeService::sendRequest(const std::string &id)
{
    // stop premature requests (before the connection is available); when TypeInfo::init
    // is called, the requests will be re-issued manually
    if (!_inited)
        return;
    
    Get get;
    Atlas::Message::MapType args;
    args["id"] = id;
    get->setArgsAsList(Atlas::Message::ListType(1, args));
    
    get->setSerialno(getNewSerialno());
    m_typeRequests.insert(get->getSerialno());
    
    m_con->send(get);
}

void TypeService::recvError(const Get& get)
{
    const Atlas::Message::MapType& args = get.getArgsAsList().front();
    Atlas::Message::MapType::const_iterator A = args.find("id");
    assert(A != args.end());
        	
    std::string typenm = A->second.asString();
    TypeInfoMap::iterator T = m_types.find(typenm);
    if (T == globalTypeMap.end())
    {
        // what the fuck? getting out of here...
        throw InvalidOperation("got ERROR(GET()) with request for unknown type: " + typenm);
    }
    
    error() << got error from server looking up type << typenm;
    // parent to root?
    T->second->_bound = true;
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
        eWarning("Unable to open Atlas spec file %s, will obtain all type data from the server", specfile.c_str());
        return;
    }
 
    debug() << "Found Atlas type data in '" << specfile << "', using for initial type info";
    
    SpecDecoder decode(this);
    Atlas::Codecs::XML specXMLCodec(specStream, &decode);
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

} // of namespace Eris
