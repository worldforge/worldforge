#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Atlas/Objects/Root.h>

#include <cassert>

using namespace Atlas;

namespace Eris {
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
	
TypeInfo::TypeInfo(const std::string &id, TypeService *ts) :
	m_bound(false),
	m_name(id),
	m_typeService(ts)
{
    if (m_name == "root")
        m_bound = true; // root node is always bound
}

TypeInfo::TypeInfo(const Atlas::Objects::Root &atype, TypeService *ts) :
    m_bound(false),
    m_name(atype.getId()),
    m_typeService(ts)
{
    if (m_name == "root")
        m_bound = true; // root node is always bound

    processTypeData(atype);
}

bool TypeInfo::isA(TypeInfoPtr tp)
{
    if (!m_bound)
        warning() << "calling isA on unbound type " << m_name;
        
    if (tp == this) // uber fast short-circuit for type equality
        return true;
	
    return m_ancestors.count(tp); // non-authorative
}

void TypeInfo::processTypeData(const Atlas::Objects::Root &atype)
{
    if (atype->getID() != m_name)
    {
        error() << "mis-targeted INFO operation for " << atype->getID() << " arrived at " << m_name;
        return;
    }
        
    const StringList& parents(atype->getParents());
    for (StringList::const_iterator P = parents.begin(); P != parents.end(); ++P)
        addParent(m_typeService->getTypeByName(*P));
	
// here we aggressively lookup child types. this tends to cane the server hard
    const StringList& children(atype->getChildren());
    for (StringList::const_iterator C = children.begin(); C != children.end(); ++C)
        addChild(m_typeService->getTypeByName(*C));
  
    setupDepends();
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
    if (m_children.count(tp)) 
        return; 	
    
    m_children.insert(tp);
    // again this will not recurse due to the termination code
    tp->addParent(this);
}

void TypeInfo::addAncestor(TypeInfoPtr tp)
{
    m_ancestors.insert(tp);
	
    const TypeInfoSet& parentAncestors = tp->m_ancestors;
    m_ancestors.insert(parentAncestors.begin(), parentAncestors.end());
	
    // tell all our childen!
    for (TypeInfoSet::iterator C=m_children.begin(); C!=m_children.end();++C)
        (*C)->addAncestor(tp);
}

void TypeInfo::validateBind()
{
    if (m_bound) return;
	
    // check all our parents
    for (TypeInfoSet::iterator P=m_parents.begin(); P!=m_parents.end();++P)
        if (!(*P)->isBound()) return;
	
    m_bound = true;
    Bound.emit();
		
    for (TypeInfoSet::iterator C=m_children.begin(); C!=m_children.end();++C)
        (*C)->validateBind();
}

} // of namespace Eris
