#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>
#include <Eris/LogStream.h>
#include <Eris/Exceptions.h>
#include <Eris/Avatar.h>
#include <Eris/Alarm.h>
#include <Eris/Task.h>

#include <wfmath/atlasconv.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/BaseObject.h>

#include <algorithm>
#include <set> 
#include <cmath>
#include <cassert>

#pragma warning(disable: 4068)  //unknown pragma

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
    m_location(NULL),
    m_id(id),
    m_stamp(-1.0),
    m_visible(false),
    m_limbo(false),
    m_updateLevel(0),
    m_hasBBox(false),
    m_moving(false),
    m_recentlyCreated(false),
    m_initialised(true)
{
    assert(m_id.size() > 0);
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
    for (TaskArray::const_iterator it = m_tasks.begin(); it != m_tasks.end(); ++it) {
        TaskRemoved(*it);
        delete *it;
    }

    if (m_moving) {
    	removeFromMovementPrediction();
    }
    
    while (!m_contents.empty()) {
      Entity *e = m_contents.back();
      e->shutdown();
      delete e;
    }
    setLocation(NULL);
    

    m_initialised = false;
}

void Entity::init(const RootEntity& ge, bool fromCreateOp)
{
    // setup initial state
    sight(ge);
    
    if (fromCreateOp)
    {
        m_recentlyCreated = true;
        new Alarm(5000, sigc::mem_fun(this, &Entity::createAlarmExpired));
    }
}

#pragma mark -

const Element& Entity::valueOfAttr(const std::string& attr) const
{
    ///first check with the instance attributes
    AttrMap::const_iterator A = m_attrs.find(attr);
    if (A == m_attrs.end())
    {
        if (m_type) {
            ///it wasn't locally defines, now check with typeinfo
            const Element* element(m_type->getAttribute(attr));
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
        ///it wasn't locally defines, now check with typeinfo
        if (m_type->getAttribute(attr) != 0) {
            return true;
        }
    }
    return false;
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
    for (TypeInfoSet::const_iterator I = typeInfo->getParents().begin(); I != typeInfo->getParents().end(); ++I) {
        fillAttributesFromType(attributes, *I);
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
    for (const Entity* e = this; e; e = e->getLocation())
        vpos = e->toLocationCoords(vpos);
        
    return vpos;
}

WFMath::Quaternion Entity::getViewOrientation() const
{
    WFMath::Quaternion vor;
	
	vor.identity();
    for (const Entity* e = this; e; e = e->getLocation())
        vor *= e->getOrientation();
        
    return vor;
}

WFMath::Point<3> Entity::getPredictedPos() const
{
    WFMath::Point<3> res = (m_moving ? m_predicted.position : m_position);
 //   if (!res.isValid())
 //   {
 //       debug() << "invalid pos for entity " << m_id << ":" << m_name;
 //   }
    return res;
}

WFMath::Vector<3> Entity::getPredictedVelocity() const
{
    return (m_moving ? m_predicted.velocity : WFMath::Vector<3>());
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
}

TypeInfoArray Entity::getUseOperations() const
{
    AttrMap::const_iterator it = m_attrs.find("operations");
    if (it == m_attrs.end()) return TypeInfoArray();
    
    if (!it->second.isList()) {
        warning() << "entity " << m_id << " has operations attr which is not a list";
        return TypeInfoArray();
    } 
    
    const ListType& opsl(it->second.asList());
    TypeInfoArray useOps;
    useOps.reserve(opsl.size());
    TypeService* ts = getTypeService();
    
    for (ListType::const_iterator i=opsl.begin(); i!=opsl.end(); ++i)
    {
        if (!i->isString())
        {
            warning() << "ignoring malformed operations list item";
            continue;
        }
    
        useOps.push_back(ts->getTypeByName(i->asString()));
    }
    
    return useOps;
}

#pragma mark -

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
    
    ///Fill with the default values from the type info
    if (includeTypeInfoAttributes && m_type) {
        fillAttributesFromType(attrs, m_type);
    }
    
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
}

void Entity::filterMoveAttrs(Atlas::Message::MapType& attrs) const
{
    attrs.erase("pos");
    attrs.erase("mode");
    attrs.erase("velocity");
    attrs.erase("orientation");
    attrs.erase("accel");
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
    if (arg->hasAttr("description")) {
        Emote.emit(arg->getAttr("description").asString());
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


#pragma mark -

void Entity::setAttr(const std::string &attr, const Element &val)
{
    beginUpdate();
    
    
    ///Check whether the attribute already has been added to the instance attributes.
    const Element* typeElement(0);
    AttrMap::const_iterator A = m_attrs.find(attr);
    if (A == m_attrs.end() && m_type) {
        ///If the attribute hasn't been defined for this instance, see if there's a typeinfo default one
        typeElement = m_type->getAttribute(attr);
    }
    
    
    const Element* newElement(0);
    ///There was no preexisting attribute either in this instance or in the TypeInfo, just insert it.
    if (A == m_attrs.end() && typeElement == 0) {
        std::pair<AttrMap::const_iterator, bool> I = m_attrs.insert(AttrMap::value_type(attr, val));
        newElement = &(I.first->second);
    } else {
        ///Create an instance specific attribute
        Element& target = m_attrs[attr];
        ///If there already existed an instance attribute copy from that
        newElement = &target;
        if (A == m_attrs.end()) {
            ///Copy from the type info attribute
            target = *typeElement;
        }
        mergeOrCopyElement(val, target);
        
    }
    nativeAttrChanged(attr, *newElement);
    onAttrChanged(attr, *newElement);

    // fire observers
    
    ObserverMap::const_iterator obs = m_observers.find(attr);
    if (obs != m_observers.end()) {
        obs->second.emit(*newElement);
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
        m_stamp = v.asFloat();
        return true;
    } else if (attr == "pos") {
        m_position.fromAtlas(v);
        return true;
    } else if (attr == "velocity") {
        m_velocity.fromAtlas(v);
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
        m_bbox.fromAtlas(v);
        m_hasBBox = true;
        return true;
    } else if ((attr == "loc") ||(attr == "contains")) {
        throw InvalidOperation("tried to set loc or contains via setProperty");
    } else if (attr == "tasks") {
        updateTasks(v);
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
            m_modifiedAttrs.count("orientation"))
        {
            m_lastMoveTime = TimeStamp::now();
            
            const WFMath::Vector<3> & velocity = getVelocity();
            bool nowMoving = (velocity.isValid() && (velocity.sqrMag() > 1e-3));
            if (nowMoving != m_moving) setMoving(nowMoving);
            
            onMoved();
        }
        
        m_modifiedAttrs.clear();
    }
}

static int findTaskByName(const TaskArray& a, const std::string& nm)
{
    for (unsigned int t=0; t < a.size(); ++t)
    {
        if (a[t]->name() == nm) return t;
    }
    
    return -1;
}

void Entity::updateTasks(const Element& e)
{
    if (!e.isList()) return; // malformed
    const ListType& taskList(e.asList());
    
    /*
    Explanation:
    
    We take a copy of the current pointer-to-task array, clear out the 'real'
    task array, and then walk the array defined by Atlas. For each pre-existing
    task still specified in the Atlas data, we find the old task in the copied
    array, re-add it to the main list, and NULL the entry in the copied list.
    
    At the end of iterating the Atlas data, any pointers still valid in the
    copied list are dead, and can be deleted.
    */
    
    TaskArray previousTasks(m_tasks);
    m_tasks.clear();
    
    for (unsigned int i=0; i<taskList.size(); ++i)
    {
        const MapType& tkmap(taskList[i].asMap());
        MapType::const_iterator it = tkmap.find("name");
        if (it == tkmap.end())
        {
            error() << "task without name";
            continue;
        }
        
        int index = findTaskByName(previousTasks, it->second.asString());
        Task *task;
        
        if (index < 0)
        {   // not found, create a new one
            task = new Task(this, it->second.asString());
            onTaskAdded(task);
        } else {
            task = previousTasks[index];
            previousTasks[index] = NULL;
        }
        
        m_tasks.push_back(task);
        task->updateFromAtlas(tkmap);
    } // of Atlas-specified tasks iteration
    
    for (unsigned int d=0; d<previousTasks.size(); ++d)
    {
        if (!previousTasks[d]) continue;
        
        TaskRemoved(previousTasks[d]);
        delete previousTasks[d];
    } // of previous-task cleanup iteration
}

void Entity::removeTask(Task* t)
{
    TaskArray::iterator it = std::find(m_tasks.begin(), m_tasks.end(), t);
    if (it == m_tasks.end())
    {
        error() << "unknown task " << t->name() << " on entity " << this;
        return;
    }
    
    m_tasks.erase(it);
    TaskRemoved(t);
}

#pragma mark -

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
		m_location = NULL;
		assert(m_visible == false);
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

#pragma mark -

void Entity::buildEntityDictFromContents(IdEntityMap& dict)
{
    for (unsigned int C=0; C < m_contents.size(); ++C) {
        Entity* child = m_contents[C];
        dict[child->getId()] = child;
    }
}

void Entity::setContentsFromAtlas(const StringList& contents)
{
// convert existing contents into a map, for fast membership tests
    IdEntityMap oldContents;
    buildEntityDictFromContents(oldContents);
    
// iterate over new contents
    for (StringList::const_iterator I=contents.begin(); I != contents.end(); ++I) {
        Entity* child = NULL;
        
        IdEntityMap::iterator J = oldContents.find(*I);
        if (J != oldContents.end()) {
            child = J->second;
            assert(child->getLocation() == this);
            oldContents.erase(J);
        } else {
            child = getEntity(*I);
            if (!child) {
            	continue;
            }
            
            if (child->m_limbo) {
                assert(child->m_visible == false);
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
    for (IdEntityMap::const_iterator J = oldContents.begin(); J != oldContents.end(); ++J) {
        J->second->setVisible(false);
    }
}

bool Entity::hasChild(const std::string& eid) const
{
    for (EntityArray::const_iterator C=m_contents.begin(); C != m_contents.end(); ++C) {
        if ((*C)->getId() == eid) return true;
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
    
    for (EntityArray::iterator C=m_contents.begin(); C != m_contents.end(); ++C)
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

#pragma mark -
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
    
    if (m_location)
        return m_visible && m_location->isVisible();
    else
        return m_visible; // only for the root entity
}

void Entity::updateCalculatedVisibility(bool wasVisible)
{
    bool nowVisible = isVisible();
    if (nowVisible == wasVisible) return;
    
    /* the following code looks odd, so remember that only one of nowVisible and
    wasVisible can ever be true. The structure is necesary so that we fire
    Appearances top-down, but Disappearances bottom-up. */
    
    if (nowVisible) {
        onVisibilityChanged(true);
    }
    
    for (unsigned int C=0; C < m_contents.size(); ++C)
    {
        /* in case this isn't clear; if we were visible, then child visibility
        was simply it's locally set value; if we were invisible, that the
        child must also have been invisible too. */
        bool childWasVisible = wasVisible ? m_contents[C]->m_visible : false;
        m_contents[C]->updateCalculatedVisibility(childWasVisible);
    }
    
    if (wasVisible) {
        onVisibilityChanged(false);
    }
}

void Entity::onVisibilityChanged(bool vis)
{
    VisibilityChanged.emit(vis);
}

void Entity::createAlarmExpired()
{
    m_recentlyCreated = false;
}

} // of namespace 
