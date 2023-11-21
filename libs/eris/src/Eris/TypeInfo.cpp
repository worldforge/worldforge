#include <utility>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "TypeInfo.h"
#include "Log.h"
#include "Exceptions.h"
#include "TypeService.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>
#include <algorithm>

using Atlas::Objects::Root;
using namespace Atlas::Objects::Operation;

namespace Eris {
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
	
TypeInfo::TypeInfo(std::string id, TypeService &ts) :
    m_parent(nullptr),
    m_bound(false),
    m_name(std::move(id)),
    m_typeService(ts)
{
    if (m_name == "root") {
		m_bound = true; // root node is always bound
	}
}

TypeInfo::TypeInfo(const Root &atype, TypeService &ts) :
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

bool TypeInfo::isA(TypeInfo* tp) const
{
    if (!m_bound) {
        logger->warn("calling isA on unbound type {}", m_name);
    }
    
    // uber fast short-circuit for type equality
    if (tp == this) {
        return true;
    }
	
    return m_ancestors.find(tp) != m_ancestors.end(); // non-authorative if not bound
}

bool TypeInfo::isA(const std::string& typeName) const
{
    if (!m_bound) {
        logger->warn("calling isA on unbound type {}", m_name);
    }
    if (m_name == typeName) {
        return true;
    }

    auto I = std::find_if(m_ancestors.begin(), m_ancestors.end(), [&](const TypeInfo* typeInfo){ return typeInfo->m_name == typeName;});
    return I != m_ancestors.end();
}


bool TypeInfo::hasUnresolvedChildren() const
{
    return !m_unresolvedChildren.empty();
}

void TypeInfo::resolveChildren()
{
    if (m_unresolvedChildren.empty()) {
        logger->error("Type {} has no unresolved children", m_name);
        return;
    }

    auto uchildren = m_unresolvedChildren;
    for (const auto& child : uchildren) {
        addChild(m_typeService.getTypeByName(child));
    }
    
    assert(m_unresolvedChildren.empty());
}

void TypeInfo::processTypeData(const Root &atype)
{

    if (atype->getId() != m_name) {
        logger->error("mis-targeted INFO operation for {} arrived at {}", atype->getId(), m_name);
        return;
    }


    if (atype->hasAttr("children"))
    {
        const Atlas::Message::Element childElem(atype->getAttr("children"));
        if (!childElem.isList()) {
            logger->warn("'children' element is not of list type when processing entity type {}.", m_name);
        } else {
            const Atlas::Message::ListType & children(childElem.asList());

            for (const auto& childElement : children) {
            	if (childElement.isString()) {
					TypeInfo* child = m_typeService.findTypeByName(childElement.String());
					// if the child was already known, don't add to unresolved
					if (child && m_children.find(child) != m_children.end()) {
						continue;
					}

					m_unresolvedChildren.insert(childElement.String());
				}
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
        setParent(m_typeService.getTypeByName(atype->getParent()));
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
    if (&m_typeService != &x.m_typeService)
        logger->warn("comparing TypeInfos from different type services, bad");
        
    return (m_name == x.m_name);
}

bool TypeInfo::operator<(const TypeInfo &x) const
{
    return m_name < x.m_name;
}

void TypeInfo::setParent(TypeInfo* tp)
{
    if (m_parent)
    {
        // it's critical we bail fast here to avoid infinite mutual recursion with addChild
        return;
    }
	
    if (m_ancestors.count(tp)) {
        logger->error("Adding {} as parent of {}, but already marked as ancestor", tp->m_name, m_name);
    }

    // update the gear
    m_parent = tp;
    addAncestor(tp);
	
    // note this will never recurse deep because of the fast exiting up top
    tp->addChild(this);
}

void TypeInfo::addChild(TypeInfo* tp)
{
	assert(tp);
    if (tp == this) {
        logger->error("Attempt to add {} as a child if itself",  getName());
        return;
    }
    if (tp->getName() == this->getName()) {
        logger->error("Attempt to add {} as child to identical parent ", getName());
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

void TypeInfo::addAncestor(TypeInfo* tp)
{
    // someone has reported getting into a loop here (i.e a circular inheritance
    // graph). To try and catch that, I'm putting this assert in. If / when you
    // hit it, get in touch with James.
    assert(m_children.count(tp) == 0);
    assert(m_ancestors.count(tp) == 0);
    
    m_ancestors.insert(tp);
	
    auto& parentAncestors = tp->m_ancestors;
    m_ancestors.insert(parentAncestors.begin(), parentAncestors.end());
	
    // tell all our children!
    for (auto child : m_children) {
		child->addAncestor(tp);
    }
}

void TypeInfo::extractDefaultProperties(const Atlas::Objects::Root& atype)
{
    ///See if there's any default properties defined, and if so make a copy, accessible through "getProperties()".
    if (atype->hasAttr("properties")) {
        auto propertiesElement = atype->getAttr("properties");
        if (!propertiesElement.isMap()) {
            logger->warn("'properties' element is not of map type when processing entity type {}.", m_name);
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
    m_typeService.sendRequest(m_name);
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
    m_typeService.BoundType.emit(this);
    
    for (auto child : m_children) {
        child->validateBind();
    }
}

} // of namespace Eris
