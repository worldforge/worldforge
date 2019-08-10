#include <utility>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "TypeInfo.h"
#include "Log.h"
#include "Exceptions.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/objectFactory.h>

#include <cassert>

using Atlas::Objects::Root;
using namespace Atlas::Objects::Operation;

namespace Eris {
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
	
TypeInfo::TypeInfo(std::string id, TypeService *ts) :
    m_parent(nullptr),
    m_bound(false),
    m_name(std::move(id)),
    m_typeService(ts)
{
    if (m_name == "root")
        m_bound = true; // root node is always bound
}

TypeInfo::TypeInfo(const Root &atype, TypeService *ts) :
    m_parent(nullptr),
    m_bound(false),
    m_name(atype->getId()),
    m_typeService(ts)
{
    if (m_name == "root") {
        m_bound = true; // root node is always bound
    }

    processTypeData(atype);
}

bool TypeInfo::isA(TypeInfoPtr tp)
{
    if (!m_bound) {
        warning() << "calling isA on unbound type " << m_name;
    }
    
    // uber fast short-circuit for type equality
    if (tp == this) {
        return true;
    }
	
    return m_ancestors.count(tp) != 0; // non-authorative if not bound
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
    for (const auto& child : uchildren) {
        addChild(m_typeService->getTypeByName(child));
    }
    
    assert(m_unresolvedChildren.empty());
}

void TypeInfo::processTypeData(const Root &atype)
{

    if (atype->getId() != m_name) {
        error() << "mis-targeted INFO operation for " << atype->getId() << " arrived at " << m_name;
        return;
    }


    if (atype->hasAttr("children"))
    {
        const Atlas::Message::Element childElem(atype->getAttr("children"));
        if (!childElem.isList()) {
            warning() << "'children' element is not of list type when processing entity type " << m_name << ".";
        } else {
            const Atlas::Message::ListType & children(childElem.asList());

            for (const auto& childElement : children) {
                TypeInfo* child = m_typeService->findTypeByName(childElement.asString());
                // if the child was already known, don't add to unresolved
                if (child && m_children.count(child)) {
                    continue;
                }

                m_unresolvedChildren.insert(childElement.asString());
            }
        }
    }


    //No need to signal changes for "entities" since it's only used for creation of new entities
    Atlas::Message::Element entitiesElement;
    if (atype->copyAttr("entities", entitiesElement) == 0) {
        if (entitiesElement.isList()) {
            m_entities = std::move(entitiesElement.List());
        }
    }


    //Don't allow parent and obj type to be changed for already bound types.
    if (!m_bound) {
        setParent(m_typeService->getTypeByName(atype->getParent()));
        m_objType = atype->getObjtype();

		extractDefaultProperties(atype);

        validateBind();
    } else {
        //For already bound types we'll extract the properties and check if any changed.

        auto oldProperties = std::move(m_properties);

		extractDefaultProperties(atype);

        for (auto& entry : m_properties) {
            auto oldEntryI = oldProperties.find(entry.first);
            if (oldEntryI == oldProperties.end() || oldEntryI->second != entry.second) {
                PropertyChanges.emit(entry.first, entry.second);
            }

            if (oldEntryI != oldProperties.end()) {
                oldProperties.erase(oldEntryI);
            }
        }

        //If there are any old properties left they have been removed from the type, we should signal with an empty element.
        for (auto& entry : oldProperties) {
            PropertyChanges.emit(entry.first, Atlas::Message::Element());
        }

    }
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

void TypeInfo::setParent(TypeInfoPtr tp)
{
    if (m_parent)
    {
        // it's critical we bail fast here to avoid infinite mutual recursion with addChild
        return;
    }
	
    if (m_ancestors.count(tp)) {
        error() << "Adding " << tp->m_name << " as parent of " << m_name << ", but already marked as ancestor";
    }

    // update the gear
    m_parent = tp;
    addAncestor(tp);
	
    // note this will never recurse deep because of the fast exiting up top
    tp->addChild(this);
}

void TypeInfo::addChild(TypeInfoPtr tp)
{
	assert(tp);
    if (tp == this) {
        error() << "Attempt to add " << getName() << " as a child if itself";
        return;
    }
    if (tp->getName() == this->getName()) {
        error() << "Attempt to add " << getName() << " as child to identical parent ";
        return;
    }
    
    if (m_children.count(tp)) {
        return;
    }
    m_unresolvedChildren.erase(tp->getName());
    
    m_children.insert(tp);
    // again this will not recurse due to the termination code
    tp->setParent(this);
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
    for (auto child : m_children) {
		child->addAncestor(tp);
    }
}

void TypeInfo::extractDefaultProperties(const Atlas::Objects::Root& atype)
{
    ///See if there's any default properties defined, and if so make a copy, accessible through "getProperties()".
    if (atype->hasAttr("properties")) {
        const Atlas::Message::Element propertiesElement(atype->getAttr("properties"));
        if (!propertiesElement.isMap()) {
            warning() << "'properties' element is not of map type when processing entity type " << m_name << ".";
        } else {
			m_properties = propertiesElement.Map();
        }
    }
}


const Atlas::Message::Element* TypeInfo::getProperty(const std::string& propertyName) const
{
    ///first check with the local properties
    auto A = m_properties.find(propertyName);
    if (A != m_properties.end()) {
        return &(A->second);
    } else {
        ///it wasn't locally defined check with the parent
        if (getParent()) {
            const Atlas::Message::Element* element(getParent()->getProperty(propertyName));
            if (element) {
                return element;
            }
        }
    }
    return nullptr;
}

void TypeInfo::setProperty(const std::string& propertyName, const Atlas::Message::Element& element)
{
    onPropertyChanges(propertyName, element);
    auto I = m_properties.find(propertyName);
    if (I == m_properties.end()) {
		m_properties.insert(Atlas::Message::MapType::value_type(propertyName, element));
    } else {
        I->second = element;
    }
}

void TypeInfo::onPropertyChanges(const std::string& propertyName, const Atlas::Message::Element& element)
{
    PropertyChanges.emit(propertyName, element);
    ///Now go through all children, and only make them emit the event if they themselves doesn't have an property by this name (which thus overrides this).
    for (auto child : getChildren()) {
        Atlas::Message::MapType::const_iterator J = child->m_properties.find(propertyName);
        if (J == child->m_properties.end()) {
			child->onPropertyChanges(propertyName, element);
        }
    }
}

void TypeInfo::refresh() {
    m_typeService->sendRequest(m_name);
}


void TypeInfo::validateBind()
{
    if (m_bound) return;
	
    // check all our parents
    if (m_parent) {
        if (!m_parent->isBound()) return;
    }

    m_bound = true;
         
    Bound.emit();
    m_typeService->BoundType.emit(this);
    
    for (auto child : m_children) {
        child->validateBind();
    }
}

} // of namespace Eris
