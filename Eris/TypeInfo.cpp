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

#pragma warning(disable: 4068)  //unknown pragma

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
    if (m_name == "root") m_bound = true; // root node is always bound

    processTypeData(atype);
}

bool TypeInfo::isA(TypeInfoPtr tp)
{
    if (!m_bound) warning() << "calling isA on unbound type " << m_name;
    
    // uber fast short-circuit for type equality
    if (tp == this) return true;
	
    return m_ancestors.count(tp); // non-authorative if not bound
}

bool TypeInfo::hasUnresolvedChildren() const
{
    return !m_unresolvedChildren.empty();
}

void TypeInfo::resolveChildren()
{
    if (m_unresolvedChildren.empty()) {
        error() << "Type " << m_name << " has no unresolved children";
        return;
    }
    
    StringSet uchildren(m_unresolvedChildren);
    for (StringSet::const_iterator it = uchildren.begin(); it != uchildren.end(); ++it) {
        addChild(m_typeService->getTypeByName(*it));
    }
    
    assert(m_unresolvedChildren.empty());
}

#pragma mark - 

void TypeInfo::processTypeData(const Root &atype)
{

    if (atype->getId() != m_name) {
        error() << "mis-targeted INFO operation for " << atype->getId() << " arrived at " << m_name;
        return;
    }
        
    const StringList& parents(atype->getParents());
    for (StringList::const_iterator P = parents.begin(); P != parents.end(); ++P)
        addParent(m_typeService->getTypeByName(*P));
	
    if (atype->hasAttr("children"))
    {
        const Atlas::Message::Element childElem(atype->getAttr("children"));
        if (!childElem.isList()) {
            warning() << "'children' element is not of list type when processing entity type " << m_name << ".";
        } else {
            const Atlas::Message::ListType & children(childElem.asList());

            for (Atlas::Message::ListType::const_iterator C = children.begin(); C != children.end(); ++C) {
                TypeInfo* child = m_typeService->findTypeByName(C->asString());
                // if the child was already known, don't add to unresolved
                if (child && m_children.count(child)) continue;

                m_unresolvedChildren.insert(C->asString());
            }
        }
    }
    
    extractDefaultAttributes(atype);
      
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
    
    if (m_children.count(tp)) return; 	
    m_unresolvedChildren.erase(tp->getName());
    
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
    for (TypeInfoSet::const_iterator C=m_children.begin(); C!=m_children.end();++C) {
        (*C)->addAncestor(tp);
    }
}

void TypeInfo::extractDefaultAttributes(const Atlas::Objects::Root& atype)
{
    ///See if there's any default attributes defined, and if so make a copy, accessable through "getAttributes()".
    if (atype->hasAttr("attributes")) {
        const Atlas::Message::Element attrsElement(atype->getAttr("attributes"));
        if (!attrsElement.isMap()) {
            warning() << "'attributes' element is not of map type when processing entity type " << m_name << ".";
        } else {
            const Atlas::Message::MapType& attrsMap(attrsElement.asMap());
            for (Atlas::Message::MapType::const_iterator I = attrsMap.begin(); I != attrsMap.end(); ++I)
            {
                std::string attributeName(I->first);
                if (I->second.isMap()) {
                    const Atlas::Message::MapType& innerAttributeMap(I->second.asMap());
                    Atlas::Message::MapType::const_iterator J = innerAttributeMap.find("default");
                    if (J != innerAttributeMap.end()) {
                        ///Only add the attribute if it's marked as publicly visible (will there ever be private attributes sent over the wire?).
                        Atlas::Message::MapType::const_iterator visibilityI = innerAttributeMap.find("visibility");
                        if (visibilityI != innerAttributeMap.end()) {
                            if (visibilityI->second.isString()) {
                                if (visibilityI->second.asString() == "public") {
                                    m_attributes.insert(Atlas::Message::MapType::value_type(attributeName, J->second));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


const Atlas::Message::Element* TypeInfo::getAttribute(const std::string& attributeName) const
{
    static Atlas::Message::Element* emptyElement(0);
    
    ///first check with the local attributes
    Atlas::Message::MapType::const_iterator A = m_attributes.find(attributeName);
    if (A != m_attributes.end()) {
        return &(A->second);
    } else {
        ///it wasn't locally defined check with the parents
        for (TypeInfoSet::const_iterator I = getParents().begin(); I != getParents().end(); ++I) {
            const Atlas::Message::Element* element((*I)->getAttribute(attributeName));
            if (element) {
                return element;
            }
        }
    }
    return emptyElement;
}

void TypeInfo::setAttribute(const std::string& attributeName, const Atlas::Message::Element& element)
{
    onAttributeChanges(attributeName, element);
    Atlas::Message::MapType::iterator I = m_attributes.find(attributeName);
    if (I == m_attributes.end()) {
        m_attributes.insert(Atlas::Message::MapType::value_type(attributeName, element));
    } else {
        I->second = element;
    }
}


void TypeInfo::onAttributeChanges(const std::string& attributeName, const Atlas::Message::Element& element)
{
    AttributeChanges.emit(attributeName, element);
    ///Now go through all children, and only make them emit the event if they themselves doesn't have an attribute by this name (which thus overrides this).
    for (TypeInfoSet::const_iterator I = getChildren().begin(); I != getChildren().end(); ++I) {
        Atlas::Message::MapType::const_iterator J = (*I)->m_attributes.find(attributeName);
        if (J == (*I)->m_attributes.end()) {
            (*I)->onAttributeChanges(attributeName, element);
        }
    }
}

void TypeInfo::validateBind()
{
    if (m_bound) return;
	
    // check all our parents
    for (TypeInfoSet::const_iterator P=m_parents.begin(); P!=m_parents.end();++P) {
        if (!(*P)->isBound()) return;
	}
    
    m_bound = true;
         
    Bound.emit();
    m_typeService->BoundType.emit(this);
    
    for (TypeInfoSet::const_iterator C=m_children.begin(); C!=m_children.end();++C) {
        (*C)->validateBind();
    }
}

} // of namespace Eris
