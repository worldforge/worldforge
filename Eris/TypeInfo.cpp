#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/objectFactory.h>

#include <cassert>

using Atlas::Objects::Root;
using namespace Atlas::Objects::Operation;

namespace Eris {
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
	
TypeInfo::TypeInfo(const std::string &id, TypeService *ts) :
    m_bound(false),
    m_name(id),
    m_atlasClassNo(0),
    m_moveCount(0),
    m_typeService(ts)
{
    if (m_name == "root")
        m_bound = true; // root node is always bound
}

TypeInfo::TypeInfo(const Root &atype, TypeService *ts) :
    m_bound(false),
    m_name(atype->getId()),
    m_moveCount(0),
    m_typeService(ts)
{
    if (m_name == "root")
        m_bound = true; // root node is always bound

    processTypeData(atype);
}

bool TypeInfo::isA(TypeInfoPtr tp)
{
    if (!m_bound) warning() << "calling isA on unbound type " << m_name;
    
    // uber fast short-circuit for type equality
    if (tp == this) return true;
	
    return m_ancestors.count(tp); // non-authorative if not bound
}

void TypeInfo::processTypeData(const Root &atype)
{
    if (atype->getId() != m_name) {
        error() << "mis-targeted INFO operation for " << atype->getId() << " arrived at " << m_name;
        return;
    }
        
    const StringList& parents(atype->getParents());
    for (StringList::const_iterator P = parents.begin(); P != parents.end(); ++P)
        addParent(m_typeService->getTypeByName(*P));
	
// here we aggressively lookup child types. this tends to cane the server hard
    if (atype->hasAttr("children"))
    {
        const Atlas::Message::Element childElem(atype->getAttr("children"));
        const Atlas::Message::ListType & children(childElem.asList());
        
        for (Atlas::Message::ListType::const_iterator C = children.begin(); C != children.end(); ++C)
            addChild(m_typeService->getTypeByName(C->asString()));
    }
      
    validateBind();
}

bool TypeInfo::operator==(const TypeInfo &x) const
{
    if (m_typeService != x.m_typeService)
        warning() << "comparing TypeInfos from different type services, bad";
        
    return (m_name == x.m_name);
}

bool TypeInfo::operator<(const TypeInfo &x) const
{
    return m_name < x.m_name;
}

void TypeInfo::addParent(TypeInfoPtr tp)
{
    if (m_parents.count(tp))
    {
        // it's critcial we bail fast here to avoid infitite mutual recursion with addChild
        return;
    }
	
    if (m_ancestors.count(tp))
	error() << "Adding " << tp->m_name << " as parent of " << m_name << ", but already marked as ancestor";
    
    // update the gear
    m_parents.insert(tp);
    addAncestor(tp);
	
    // note this will never recurse deep becuase of the fast exiting up top
    tp->addChild(this);
}

void TypeInfo::addChild(TypeInfoPtr tp)
{
    if (tp == this) {
        error() << "Attempt to add " << getName() << " as a child if itself";
        return;
    }
    if (tp->getName() == this->getName()) {
        error() << "Attempt to add " << getName() << " as child to identical parent ";
        return;
    }
    if (m_children.count(tp)) 
        return; 	
    
    m_children.insert(tp);
    // again this will not recurse due to the termination code
    tp->addParent(this);
}

void TypeInfo::addAncestor(TypeInfoPtr tp)
{
    // someone has reported getting into a loop here (i.e a circular inheritance
    // graph). To try and catch that, I'm putting this assert in. If / when you
    // hit it, get in touch with James.
    assert(m_children.count(tp) == 0);
    assert(m_ancestors.count(tp) == 0);
    
    m_ancestors.insert(tp);
	
    const TypeInfoSet& parentAncestors = tp->m_ancestors;
    m_ancestors.insert(parentAncestors.begin(), parentAncestors.end());
	
    // tell all our childen!
    for (TypeInfoSet::iterator C=m_children.begin(); C!=m_children.end();++C)
        (*C)->addAncestor(tp);
}

static Atlas::Objects::Root gameEntityFactory()
{
    return Atlas::Objects::Entity::GameEntity();
}

static Atlas::Objects::Root adminEntityFactory()
{
    return Atlas::Objects::Entity::AdminEntity();
}

static Atlas::Objects::Root actionFactory()
{
    return Atlas::Objects::Operation::Action();
}

void TypeInfo::validateBind()
{
    if (m_bound) return;
	
    // check all our parents
    for (TypeInfoSet::iterator P=m_parents.begin(); P!=m_parents.end();++P)
        if (!(*P)->isBound()) return;
	
    m_bound = true;
     
    if (!Atlas::Objects::objectFactory.hasFactory(m_name))
    {
        // identify the most accurate C++ base type and use as the factory
        if (isA(m_typeService->getTypeByName("game_entity"))) {
            m_atlasClassNo = Atlas::Objects::objectFactory.addFactory(m_name, &gameEntityFactory);
        } else if (isA(m_typeService->getTypeByName("admin_entity"))) {
            m_atlasClassNo = Atlas::Objects::objectFactory.addFactory(m_name, &adminEntityFactory);
        } else if (isA(m_typeService->getTypeByName("action"))) {
            m_atlasClassNo = Atlas::Objects::objectFactory.addFactory(m_name, &actionFactory);
        } else {
            // screwed
            error() << "type " << m_name <<  " doesn't inherit game_entity or admin_entity";
            for (TypeInfoSet::iterator P=m_ancestors.begin(); P!=m_ancestors.end();++P)
                debug() << m_name << " has ancestor " << (*P)->getName() << " @ " << *P;
        }
    }
    
    Bound.emit(this);
        
    for (TypeInfoSet::iterator C=m_children.begin(); C!=m_children.end();++C)
        (*C)->validateBind();
}

} // of namespace Eris
