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

Entity::Entity(const std::string& id, TypeInfo* ty) :
    m_type(ty),
    m_location(nullptr),
    m_id(id),
    m_stamp(-1.0f),
    m_visible(false),
    m_limbo(false),
    m_updateLevel(0),
    m_hasBBox(false),
    m_moving(false),
    m_recentlyCreated(false),
    m_initialised(true)
{
    assert(!m_id.empty());
    m_orientation.identity();
    
    
    if (m_type) {
        m_type->AttributeChanges.connect(sigc::mem_fun(this, &Entity::typeInfo_AttributeChanges));
    }
}

Entity::~Entity()
{
    assert(m_initialised == false);   
}

void Entity::shutdown()
{
    BeingDeleted.emit();

    //Delete any lingering tasks.
    for (auto& entry : m_tasks) {
        TaskRemoved(entry.second);
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

const Element& Entity::valueOfAttr(const std::string& attr) const
{
    ///first check with the instance attributes
	auto A = m_attrs.find(attr);
    if (A == m_attrs.end())
    {
        if (m_type) {
            ///it wasn't locally defined, now check with typeinfo
            const Element* element = m_type->getAttribute(attr);
            if (element) {
                return *element;
            }
        }
        error() << "did getAttr(" << attr << ") on entity " << m_id << " which has no such attr";
        throw InvalidOperation("no such attribute " + attr);
    } else {
        return A->second;
    }
}

bool Entity::hasAttr(const std::string& attr) const
{
    ///first check with the instance attributes
    if (m_attrs.count(attr) > 0) {
        return true;
    } else if (m_type) {
        ///it wasn't locally defined, now check with typeinfo
        if (m_type->getAttribute(attr) != 0) {
            return true;
        }
    }
    return false;
}

const Element* Entity::ptrOfAttr(const std::string& attr) const
{
    ///first check with the instance attributes
	auto A = m_attrs.find(attr);
    if (A == m_attrs.end())
    {
        if (m_type) {
            ///it wasn't locally defined, now check with typeinfo
            const Element* element = m_type->getAttribute(attr);
            if (element) {
                return element;
            }
        }
        return nullptr;
    } else {
        return &A->second;
    }
}


const Entity::AttrMap Entity::getAttributes() const
{
    ///Merge both the local attributes and the type default attributes.
    AttrMap attributes;
    attributes.insert(m_attrs.begin(), m_attrs.end());
    if (m_type) {
        fillAttributesFromType(attributes, m_type);
    }
    return attributes;
}

const Entity::AttrMap& Entity::getInstanceAttributes() const
{
    return m_attrs;
}

void Entity::fillAttributesFromType(Entity::AttrMap& attributes, TypeInfo* typeInfo) const
{
    attributes.insert(typeInfo->getAttributes().begin(), typeInfo->getAttributes().end());
    ///Make sure to fill from the closest attributes first, as insert won't replace an existing value

	if (typeInfo->getParent()) {
		fillAttributesFromType(attributes, typeInfo->getParent());
	}

}

sigc::connection Entity::observe(const std::string& attr, const AttrChangedSlot& slot)
{
    // sometimes, I realize how great SigC++ is
    return m_observers[attr].connect(slot);
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

void Entity::updatePredictedState(const WFMath::TimeStamp& t)
{
    assert(isMoving());
    
    float dt = (t - m_lastMoveTime).milliseconds() / 1000.0f; 
    
    if (m_acc.isValid()) {
        m_predicted.velocity = m_velocity + (m_acc * dt);
        m_predicted.position = m_position + (m_velocity * dt) + (m_acc * 0.5f * dt * dt);
    } else {
        m_predicted.velocity = m_velocity;
        m_predicted.position = m_position + (m_velocity * dt);
    }
    if (m_angularVelocity.isValid() && m_angularMag != .0f) {
        m_predicted.orientation = m_orientation * WFMath::Quaternion(m_angularVelocity, m_angularMag * dt);
    } else {
        m_predicted.orientation = m_orientation;
    }
}

TypeInfoArray Entity::getUseOperations() const
{
    const Element* attr = ptrOfAttr("operations");

    if (!attr) {
        return TypeInfoArray();
    }

    if (!attr->isList()) {
        warning() << "entity " << m_id << " has operations attr which is not a list";
        return TypeInfoArray();
    } 
    
    const ListType& opsl(attr->List());
    TypeInfoArray useOps;
    useOps.reserve(opsl.size());
    TypeService* ts = getTypeService();
    
    for (auto& op : opsl) {
        if (!op.isString()) {
            warning() << "ignoring malformed operations list item";
            continue;
        }
    
        useOps.push_back(ts->getTypeByName(op.String()));
    }
    
    return useOps;
}

void Entity::sight(const RootEntity &ge)
{    
    if (!ge->isDefaultLoc()) setLocationFromAtlas(ge->getLoc());
    
    setContentsFromAtlas(ge->getContains());    
    setFromRoot(ge, true, true);
}

void Entity::setFromRoot(const Root& obj, bool allowMove, bool includeTypeInfoAttributes)
{	
    beginUpdate();
    
    Atlas::Message::MapType attrs;
    obj->addToMessage(attrs);
    Atlas::Message::MapType::const_iterator A;
    
    attrs.erase("loc");
    attrs.erase("id");
    attrs.erase("contains");
    
    if (!allowMove) filterMoveAttrs(attrs);
    
    for (A = attrs.begin(); A != attrs.end(); ++A) {
        // see if the value in the sight matches the exsiting value
        AttrMap::const_iterator I = m_attrs.find(A->first);
        if ((I != m_attrs.end()) && (I->second == A->second)) continue;

        setAttr(A->first, A->second);
    }
    
    endUpdate();

    //Add any values found in the type, if they aren't defined in the entity already.
    if (includeTypeInfoAttributes && m_type) {
        Atlas::Message::MapType typeAttributes;
        fillAttributesFromType(typeAttributes, m_type);
        for (auto& entry : typeAttributes) {
            attrChangedFromTypeInfo(entry.first, entry.second);
        }
    }

}

void Entity::filterMoveAttrs(Atlas::Message::MapType& attrs) const
{
    attrs.erase("pos");
    attrs.erase("mode");
    attrs.erase("velocity");
    attrs.erase("orientation");
    attrs.erase("accel");
    attrs.erase("angular");
}

void Entity::onTalk(const Atlas::Objects::Operation::RootOperation& talk)
{
    const std::vector<Root>& talkArgs = talk->getArgs();
    if (talkArgs.empty())
    {
        warning() << "entity " << getId() << " got sound(talk) with no args";
        return;
    }

    Say.emit(talkArgs.front());
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
        updatePredictedState(m_lastMoveTime);
    }
    Moved.emit();
}

void Entity::onAction(const Atlas::Objects::Operation::RootOperation& arg)
{
    Acted.emit(arg);
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
        addToMovementPredition();
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

void Entity::onTaskAdded(Task* task)
{
	TaskAdded(task);
}


void Entity::setAttr(const std::string &attr, const Element &val)
{
    beginUpdate();
    
    m_attrs[attr] = val;

    nativeAttrChanged(attr, val);
    onAttrChanged(attr, val);

    // fire observers
    
    auto obs = m_observers.find(attr);
    if (obs != m_observers.end()) {
        obs->second.emit(val);
    }

    addToUpdate(attr);
    endUpdate();
}

bool Entity::nativeAttrChanged(const std::string& attr, const Element& v)
{
    // in the future, hash these names to a compile-time integer index, and
    // make this a switch statement. The same indice could also be used
    // in endUpdate
    
    if (attr == "name") {
        m_name = v.asString();
        return true;
    } else if (attr == "stamp") {
        m_stamp = (float)v.asFloat();
        return true;
    } else if (attr == "pos") {
        m_position.fromAtlas(v);
        return true;
    } else if (attr == "velocity") {
        m_velocity.fromAtlas(v);
        return true;
    } else if (attr == "angular") {
        m_angularVelocity.fromAtlas(v);
        m_angularMag = m_angularVelocity.mag();
        return true;
    } else if (attr == "accel") {
        m_acc.fromAtlas(v);
        return true;
    } else if (attr == "orientation") {
        m_orientation.fromAtlas(v);
        return true;
    } else if (attr == "description") {
        m_description = v.asString();
        return true;
    } else if (attr == "bbox") {
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
    } else if ((attr == "loc") ||(attr == "contains")) {
        throw InvalidOperation("tried to set loc or contains via setProperty");
    } else if (attr == "tasks") {
        updateTasks(v);
        return true;
    } else if (attr == "scale") {
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

void Entity::onAttrChanged(const std::string&, const Element&)
{
    // no-op by default
}


void Entity::typeInfo_AttributeChanges(const std::string& attributeName, const Atlas::Message::Element& element)
{
    attrChangedFromTypeInfo(attributeName, element);
}

void Entity::attrChangedFromTypeInfo(const std::string& attributeName, const Atlas::Message::Element& element)
{
    ///Only fire the events if there's no attribute already defined for this entity
    if (m_attrs.count(attributeName) == 0) {
        beginUpdate();
        nativeAttrChanged(attributeName, element);
        onAttrChanged(attributeName, element);
    
        // fire observers
        
        ObserverMap::const_iterator obs = m_observers.find(attributeName);
        if (obs != m_observers.end()) {
            obs->second.emit(element);
        }
    
        addToUpdate(attributeName);
        endUpdate();
    }
}


void Entity::beginUpdate()
{
    ++m_updateLevel;
}

void Entity::addToUpdate(const std::string& attr)
{
    assert(m_updateLevel > 0);
    m_modifiedAttrs.insert(attr);
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
        Changed.emit(m_modifiedAttrs);
        
        if (m_modifiedAttrs.count("pos") || 
            m_modifiedAttrs.count("velocity") ||
            m_modifiedAttrs.count("orientation") ||
            m_modifiedAttrs.count("angular"))
        {
            m_lastMoveTime = TimeStamp::now();
            
            const WFMath::Vector<3> & velocity = getVelocity();
            bool nowMoving = (velocity.isValid() && (velocity.sqrMag() > 1e-3)) || (m_angularVelocity.isValid() && m_angularVelocity != WFMath::Vector<3>::ZERO());
            if (nowMoving != m_moving) setMoving(nowMoving);
            
            onMoved();
        }
        
        m_modifiedAttrs.clear();
    }
}


void Entity::updateTasks(const Element& e)
{
    if (e.isNone()) {
        for (auto& entry : m_tasks) {
            TaskRemoved(entry.second);
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
        
        if (tasksI == previousTasks.end())
        {   // not found, create a new one
            task = new Task(this, it->second.asString());
            onTaskAdded(task);
        } else {
            task = tasksI->second;
            previousTasks.erase(entry.first);
        }
        
        m_tasks.emplace(entry.first, task);
        task->updateFromAtlas(tkmap);
    } // of Atlas-specified tasks iteration
    
    for (auto& entry : previousTasks) {

        if (entry.second) {
            TaskRemoved(entry.second);
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

bool Entity::extractEntityId(const Atlas::Message::Element& element, std::string& id)
{
    if (element.isString()) {
        id = element.asString();
        return true;
    } else if (element.isMap()) {
        auto I = element.asMap().find("$eid");
        if (I != element.asMap().end() && I->second.isString()) {
            id = I->second.asString();
            return true;
        }
    }
    return false;

}


} // of namespace 
