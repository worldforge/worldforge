#include <utility>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Entity.h"
#include "Connection.h"
#include "TypeInfo.h"
#include "LogStream.h"
#include "Exceptions.h"
#include "Avatar.h"
#include "Task.h"

#include <wfmath/atlasconv.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/BaseObject.h>

#include <algorithm>
#include <set> 
#include <cmath>
#include <cassert>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::smart_static_cast;
using Atlas::Objects::smart_dynamic_cast;

using WFMath::TimeStamp;
using WFMath::TimeDiff;

namespace Eris {

Entity::Entity(std::string id, TypeInfo* ty) :
    m_type(ty),
    m_location(nullptr),
    m_id(std::move(id)),
    m_stamp(-1.0f),
    m_visible(false),
    m_limbo(false),
	m_angularMag(0),
    m_updateLevel(0),
    m_hasBBox(false),
    m_moving(false),
    m_recentlyCreated(false),
    m_initialised(true)
{
    assert(!m_id.empty());
    m_orientation.identity();
    
    
    if (m_type) {
        m_type->PropertyChanges.connect(sigc::mem_fun(this, &Entity::typeInfo_PropertyChanges));
    }
}

Entity::~Entity()
{
    assert(!m_initialised);
}

void Entity::shutdown()
{
    BeingDeleted.emit();

    //Delete any lingering tasks.
    for (auto& entry : m_tasks) {
        TaskRemoved(entry.first, entry.second);
        delete entry.second;
    }

    if (m_moving) {
    	removeFromMovementPrediction();
    }
    
    while (!m_contents.empty()) {
      Entity *e = m_contents.back();
      e->shutdown();
      delete e;
    }
    setLocation(nullptr);
    

    m_initialised = false;
}

void Entity::init(const RootEntity& ge, bool fromCreateOp)
{
    // setup initial state
    sight(ge);
    
    if (fromCreateOp)
    {
        m_recentlyCreated = true;
    }
}

const Element& Entity::valueOfProperty(const std::string& name) const
{
    ///first check with the instance properties
	auto A = m_properties.find(name);
    if (A == m_properties.end())
    {
        if (m_type) {
            ///it wasn't locally defined, now check with typeinfo
            const Element* element = m_type->getProperty(name);
            if (element) {
                return *element;
            }
        }
        error() << "did valueOfProperty(" << name << ") on entity " << m_id << " which has no such name";
        throw InvalidOperation("no such property " + name);
    } else {
        return A->second;
    }
}

bool Entity::hasProperty(const std::string& p) const
{
    ///first check with the instance properties
    if (m_properties.count(p) > 0) {
        return true;
    } else if (m_type) {
        ///it wasn't locally defined, now check with typeinfo
        if (m_type->getProperty(p) != nullptr) {
            return true;
        }
    }
    return false;
}

const Element* Entity::ptrOfProperty(const std::string& name) const
{
    ///first check with the instance properties
	auto A = m_properties.find(name);
    if (A == m_properties.end())
    {
        if (m_type) {
            ///it wasn't locally defined, now check with typeinfo
            const Element* element = m_type->getProperty(name);
            if (element) {
                return element;
            }
        }
        return nullptr;
    } else {
        return &A->second;
    }
}


Entity::PropertyMap Entity::getProperties() const
{
    ///Merge both the local properties and the type default properties.
    PropertyMap properties;
    properties.insert(m_properties.begin(), m_properties.end());
    if (m_type) {
		fillPropertiesFromType(properties, m_type);
    }
    return properties;
}

const Entity::PropertyMap& Entity::getInstanceProperties() const
{
    return m_properties;
}

void Entity::fillPropertiesFromType(Entity::PropertyMap& properties, TypeInfo* typeInfo) const
{
    properties.insert(typeInfo->getProperties().begin(), typeInfo->getProperties().end());
    ///Make sure to fill from the closest properties first, as insert won't replace an existing value

	if (typeInfo->getParent()) {
		fillPropertiesFromType(properties, typeInfo->getParent());
	}

}

sigc::connection Entity::observe(const std::string& propertyName, const PropertyChangedSlot& slot)
{
    // sometimes, I realize how great SigC++ is
    return m_observers[propertyName].connect(slot);
}

WFMath::Point<3> Entity::getViewPosition() const
{
    WFMath::Point<3> vpos(0.0, 0.0, 0.0);
    //If the position is invalid, we will consider it to be (0,0,0) and skip applying it.
    for (const Entity* e = this; e; e = e->getLocation()) {
        if (e->getPosition().isValid()) {
            vpos = e->toLocationCoords(vpos);
        }
    }
        
    return vpos;
}

WFMath::Quaternion Entity::getViewOrientation() const
{
    WFMath::Quaternion vor;
	
	vor.identity();
    for (const Entity* e = this; e; e = e->getLocation()) {
        vor *= e->getOrientation();
    }
        
    return vor;
}

const WFMath::Point<3>& Entity::getPredictedPos() const
{
    return (m_moving ? m_predicted.position : m_position);
}

const WFMath::Vector<3>& Entity::getPredictedVelocity() const
{
    return (m_moving ? m_predicted.velocity : m_velocity);
}

const WFMath::Quaternion& Entity::getPredictedOrientation() const
{
    return (m_moving ? m_predicted.orientation : m_orientation);
}

bool Entity::isMoving() const
{
    return m_moving;
}

void Entity::updatePredictedState(const WFMath::TimeStamp& t, float simulationSpeed)
{
    assert(isMoving());
    
    float dt = (t - m_lastMoveTime).milliseconds() / 1000.0f; 
    
    if (m_acc.isValid()) {
        m_predicted.velocity = m_velocity + (m_acc * dt * simulationSpeed);
        m_predicted.position = m_position + (m_velocity * dt * simulationSpeed) + (m_acc * 0.5f * dt * dt * simulationSpeed);
    } else {
        m_predicted.velocity = m_velocity;
        m_predicted.position = m_position + (m_velocity * dt * simulationSpeed);
    }
    if (m_angularVelocity.isValid() && m_angularMag != .0f) {
        m_predicted.orientation = m_orientation * WFMath::Quaternion(m_angularVelocity, m_angularMag * dt * simulationSpeed);
    } else {
        m_predicted.orientation = m_orientation;
    }
}

void Entity::sight(const RootEntity &ge)
{    
    if (!ge->isDefaultLoc()) setLocationFromAtlas(ge->getLoc());
    
    setContentsFromAtlas(ge->getContains());    
    setFromRoot(ge, true, true);
}

void Entity::setFromRoot(const Root& obj, bool allowMove, bool includeTypeInfoProperties)
{	
    beginUpdate();
    
    Atlas::Message::MapType properties;
    obj->addToMessage(properties);
    Atlas::Message::MapType::const_iterator A;
    
    properties.erase("loc");
    properties.erase("id");
    properties.erase("contains");
    
    if (!allowMove) filterMoveProperties(properties);
    
    for (A = properties.begin(); A != properties.end(); ++A) {
        // see if the value in the sight matches the exsiting value
        PropertyMap::const_iterator I = m_properties.find(A->first);
        if ((I != m_properties.end()) && (I->second == A->second)) continue;

		setProperty(A->first, A->second);
    }
    
    endUpdate();

    //Add any values found in the type, if they aren't defined in the entity already.
    if (includeTypeInfoProperties && m_type) {
        Atlas::Message::MapType typeProperties;
		fillPropertiesFromType(typeProperties, m_type);
        for (auto& entry : typeProperties) {
			propertyChangedFromTypeInfo(entry.first, entry.second);
        }
    }

}

void Entity::filterMoveProperties(Atlas::Message::MapType& properties) const
{
    properties.erase("pos");
    properties.erase("mode");
    properties.erase("velocity");
    properties.erase("orientation");
    properties.erase("accel");
    properties.erase("angular");
}

void Entity::onTalk(const Atlas::Objects::Operation::RootOperation& talk)
{
    const std::vector<Root>& talkArgs = talk->getArgs();
    if (talkArgs.empty())
    {
        warning() << "entity " << getId() << " got sound(talk) with no args";
        return;
    }

    for (const auto& arg: talkArgs) {
		Say.emit(arg);
    }
    Noise.emit(talk);
}

void Entity::onLocationChanged(Entity* oldLoc)
{
    LocationChanged.emit(oldLoc);
}

void Entity::onMoved()
{
    if (m_moving) {
        //We should update the predicted pos and velocity.
        updatePredictedState(m_lastMoveTime, 1.0f);
    }
    Moved.emit();
}

void Entity::onAction(const Atlas::Objects::Operation::RootOperation& arg)
{
	Acted.emit(arg);
}

void Entity::onHit(const Atlas::Objects::Operation::Hit& arg)
{
	Hit.emit(arg);
}

void Entity::onSoundAction(const Atlas::Objects::Operation::RootOperation& op)
{
    Noise.emit(op);
}

void Entity::onImaginary(const Atlas::Objects::Root& arg)
{
    Atlas::Message::Element attr;
    if (arg->copyAttr("description", attr) == 0 && attr.isString()) {
        Emote.emit(attr.asString());
    }
}

void Entity::setMoving(bool inMotion)
{
    assert(m_moving != inMotion);
    
    if (m_moving) {
    	removeFromMovementPrediction();
    }
    m_moving = inMotion;
    if (m_moving) {
        m_predicted.position = m_position;
        m_predicted.velocity = m_velocity;
        m_predicted.orientation = m_orientation;
		addToMovementPrediction();
    }
    
    Moving.emit(inMotion);
}

void Entity::onChildAdded(Entity* child)
{
    ChildAdded.emit(child);
}

void Entity::onChildRemoved(Entity* child)
{
    ChildRemoved(child);
}

void Entity::onTaskAdded(const std::string& id, Task* task)
{
	TaskAdded(id, task);
}


void Entity::setProperty(const std::string &p, const Element &v)
{
    beginUpdate();

	m_properties[p] = v;

	nativePropertyChanged(p, v);
	onPropertyChanged(p, v);

    // fire observers
    
    auto obs = m_observers.find(p);
    if (obs != m_observers.end()) {
        obs->second.emit(v);
    }

    addToUpdate(p);
    endUpdate();
}

bool Entity::nativePropertyChanged(const std::string& p, const Element& v)
{
    // in the future, hash these names to a compile-time integer index, and
    // make this a switch statement. The same indice could also be used
    // in endUpdate
    
    if (p == "name") {
        m_name = v.asString();
        return true;
    } else if (p == "stamp") {
        m_stamp = (float)v.asFloat();
        return true;
    } else if (p == "pos") {
        m_position.fromAtlas(v);
        return true;
    } else if (p == "velocity") {
        m_velocity.fromAtlas(v);
        return true;
    } else if (p == "angular") {
        m_angularVelocity.fromAtlas(v);
        m_angularMag = m_angularVelocity.mag();
        return true;
    } else if (p == "accel") {
        m_acc.fromAtlas(v);
        return true;
    } else if (p == "orientation") {
        m_orientation.fromAtlas(v);
        return true;
    } else if (p == "description") {
        m_description = v.asString();
        return true;
    } else if (p == "bbox") {
        m_bboxUnscaled.fromAtlas(v);
        m_bbox = m_bboxUnscaled;
        if (m_scale.isValid()) {
            m_bbox.lowCorner().x() *= m_scale.x();
            m_bbox.lowCorner().y() *= m_scale.y();
            m_bbox.lowCorner().z() *= m_scale.z();
            m_bbox.highCorner().x() *= m_scale.x();
            m_bbox.highCorner().y() *= m_scale.y();
            m_bbox.highCorner().z() *= m_scale.z();
        }
        m_hasBBox = true;
        return true;
    } else if ((p == "loc") || (p == "contains")) {
        throw InvalidOperation("tried to set loc or contains via setProperty");
    } else if (p == "tasks") {
        updateTasks(v);
        return true;
    } else if (p == "scale") {
        if (v.isList()) {
            if (v.List().size() == 1) {
                if (v.List().front().isNum()) {
                    float num = v.List().front().asNum();
                    m_scale = WFMath::Vector<3>(num, num, num);
                }
            } else {
                m_scale.fromAtlas(v.List());
            }
        }
        if (m_scale.isValid() && m_bboxUnscaled.isValid()) {
            m_bbox = m_bboxUnscaled;
            m_bbox.lowCorner().x() *= m_scale.x();
            m_bbox.lowCorner().y() *= m_scale.y();
            m_bbox.lowCorner().z() *= m_scale.z();
            m_bbox.highCorner().x() *= m_scale.x();
            m_bbox.highCorner().y() *= m_scale.y();
            m_bbox.highCorner().z() *= m_scale.z();
        }
        return true;
    }

    return false; // not a native property
}

void Entity::onPropertyChanged(const std::string& propertyName, const Element& v)
{
    // no-op by default
}


void Entity::typeInfo_PropertyChanges(const std::string& propertyName, const Atlas::Message::Element& element)
{
	propertyChangedFromTypeInfo(propertyName, element);
}

void Entity::propertyChangedFromTypeInfo(const std::string& propertyName, const Atlas::Message::Element& element)
{
    ///Only fire the events if there's no property already defined for this entity
    if (m_properties.count(propertyName) == 0) {
        beginUpdate();
		nativePropertyChanged(propertyName, element);
		onPropertyChanged(propertyName, element);
    
        // fire observers
        
        ObserverMap::const_iterator obs = m_observers.find(propertyName);
        if (obs != m_observers.end()) {
            obs->second.emit(element);
        }
    
        addToUpdate(propertyName);
        endUpdate();
    }
}


void Entity::beginUpdate()
{
    ++m_updateLevel;
}

void Entity::addToUpdate(const std::string& propertyName)
{
    assert(m_updateLevel > 0);
    m_modifiedProperties.insert(propertyName);
}

void Entity::endUpdate()
{
    if (m_updateLevel < 1)
    {
        error() << "mismatched begin/end update pair on entity";
        return;
    }
        
    if (--m_updateLevel == 0) // unlocking updates
    {
        Changed.emit(m_modifiedProperties);
        
        if (m_modifiedProperties.count("pos") ||
			m_modifiedProperties.count("velocity") ||
			m_modifiedProperties.count("orientation") ||
			m_modifiedProperties.count("angular"))
        {
            m_lastMoveTime = TimeStamp::now();
            
            const WFMath::Vector<3> & velocity = getVelocity();
            bool nowMoving = (velocity.isValid() && (velocity.sqrMag() > 1e-3)) || (m_angularVelocity.isValid() && m_angularVelocity != WFMath::Vector<3>::ZERO());
            if (nowMoving != m_moving) setMoving(nowMoving);
            
            onMoved();
        }
        
        m_modifiedProperties.clear();
    }
}


void Entity::updateTasks(const Element& e)
{
    if (e.isNone()) {
        for (auto& entry : m_tasks) {
            TaskRemoved(entry.first, entry.second);
            delete entry.second;
        }
        return;
    }
    if (!e.isMap()) {
        return; // malformed
    }
    auto& taskMap = e.Map();
    
    auto previousTasks = std::move(m_tasks);
    m_tasks.clear();
    
    for (auto& entry : taskMap) {
        auto& taskElement = entry.second;
        if (!taskElement.isMap()) {
            continue;
        }
        const MapType& tkmap(taskElement.Map());
		auto it = tkmap.find("name");
        if (it == tkmap.end())
        {
            error() << "task without name";
            continue;
        }
        if (!it->second.isString())
        {
            error() << "task with invalid name";
            continue;
        }

        auto tasksI = previousTasks.find(entry.first);
        Task *task;

        bool newTask = false;
        if (tasksI == previousTasks.end())
        {   // not found, create a new one
            task = new Task(this, it->second.asString());
            newTask = true;
        } else {
            task = tasksI->second;
            previousTasks.erase(entry.first);
        }
        
        m_tasks.emplace(entry.first, task);
        task->updateFromAtlas(tkmap);
        if (newTask) {
        	onTaskAdded(entry.first, task);
        }
    } // of Atlas-specified tasks iteration
    
    for (auto& entry : previousTasks) {

        if (entry.second) {
            TaskRemoved(entry.first, entry.second);
            delete entry.second;
        }
    } // of previous-task cleanup iteration
}

void Entity::setLocationFromAtlas(const std::string& locId) {
	if (locId.empty()) {
		return;
	}

	Entity* newLocation = getEntity(locId);
	if (!newLocation) {

		m_limbo = true;
		setVisible(false); // fire disappearance, VisChanged if necessary

		if (m_location) {
			removeFromLocation();
		}
		m_location = nullptr;
		assert(!m_visible);
		return;
	}

	setLocation(newLocation);
}

void Entity::setLocation(Entity* newLocation)
{
    if (newLocation == m_location) return;
        
// do the actual member updating
    bool wasVisible = isVisible();
    if (m_location) {
    	removeFromLocation();
    }
        
    Entity* oldLocation = m_location;
    m_location = newLocation;
    
    onLocationChanged(oldLocation);
    
// fire VisChanged and Appearance/Disappearance signals
    updateCalculatedVisibility(wasVisible);
    
    if (m_location) {
    	addToLocation();
    }
}

void Entity::addToLocation()
{
    assert(!m_location->hasChild(m_id));
    m_location->addChild(this);
}

void Entity::removeFromLocation()
{
    assert(m_location->hasChild(m_id));
    m_location->removeChild(this);
}

void Entity::buildEntityDictFromContents(IdEntityMap& dict)
{
    for (auto& child : m_contents) {
		dict[child->getId()] = child;
    }
}

void Entity::setContentsFromAtlas(const StringList& contents)
{
// convert existing contents into a map, for fast membership tests
    IdEntityMap oldContents;
    buildEntityDictFromContents(oldContents);
    
// iterate over new contents
    for (auto& content : contents) {
        Entity* child = nullptr;

		auto J = oldContents.find(content);
        if (J != oldContents.end()) {
            child = J->second;
            assert(child->getLocation() == this);
            oldContents.erase(J);
        } else {
            child = getEntity(content);
            if (!child) {
            	continue;
            }
            
            if (child->m_limbo) {
                assert(!child->m_visible);
                child->m_limbo = false;
            } else if (child->isVisible()) {
                // server has gone mad, it has a location, and it's visible
                error() << "got set of contents, specifying child " << child
                    << " which is currently visible in another location";
                continue;
            }
            
            /* we have found the child, update it's location */
            child->setLocation(this);
        }
    
        child->setVisible(true);
    } // of contents list iteration
    
// mark previous contents which are not in new contents as invisible
    for (auto& entry : oldContents) {
        entry.second->setVisible(false);
    }
}

bool Entity::hasChild(const std::string& eid) const
{
    for (auto& m_content : m_contents) {
        if (m_content->getId() == eid) return true;
    }
    
    return false;
}

void Entity::addChild(Entity* e)
{
    m_contents.push_back(e);
    onChildAdded(e);
    assert(e->getLocation() == this);
}

void Entity::removeChild(Entity* e)
{
    assert(e->getLocation() == this);
    
    for (auto C=m_contents.begin(); C != m_contents.end(); ++C)
    {
        if (*C == e)
        {
            m_contents.erase(C);
            onChildRemoved(e);
            return;
        }
    }
        
   error() << "child " << e->getId() << " of entity " << m_id << " not found doing remove";
}

// visiblity related methods

void Entity::setVisible(bool vis)
{
    // force visibility to false if in limbo; necessary for the character entity,
    // which otherwise gets double appearances on activation
    if (m_limbo) vis = false;
    
    bool wasVisible = isVisible(); // store before we update m_visible
    m_visible = vis;

    updateCalculatedVisibility(wasVisible);
}

bool Entity::isVisible() const
{
    if (m_limbo) return false;
    
    if (m_location) {
		return m_visible && m_location->isVisible();
	} else {
		return m_visible; // only for the root entity
	}
}

void Entity::updateCalculatedVisibility(bool wasVisible)
{
    bool nowVisible = isVisible();
    if (nowVisible == wasVisible) return;
    
    /* the following code looks odd, so remember that only one of nowVisible and
    wasVisible can ever be true. The structure is necessary so that we fire
    Appearances top-down, but Disappearances bottom-up. */
    
    if (nowVisible) {
        onVisibilityChanged(true);
    }
    
    for (auto& item : m_contents) {
        /* in case this isn't clear; if we were visible, then child visibility
        was simply it's locally set value; if we were invisible, that the
        child must also have been invisible too. */
        bool childWasVisible = wasVisible ? item->m_visible : false;
		item->updateCalculatedVisibility(childWasVisible);
    }
    
    if (wasVisible) {
        onVisibilityChanged(false);
    }
}

void Entity::onVisibilityChanged(bool vis)
{
    VisibilityChanged.emit(vis);
}

boost::optional<std::string> Entity::extractEntityId(const Atlas::Message::Element& element)
{
    if (element.isString()) {
        return element.String();
    } else if (element.isMap()) {
        auto I = element.asMap().find("$eid");
        if (I != element.asMap().end() && I->second.isString()) {
            return I->second.String();
        }
    }
    return boost::none;

}


} // of namespace 
