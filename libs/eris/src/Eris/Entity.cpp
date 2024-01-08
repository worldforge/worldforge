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
#include "TypeService.h"

#include <wfmath/atlasconv.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/BaseObject.h>

#include <fmt/ostream.h>

#include <algorithm>
#include <set>
#include <cassert>


template<>
struct fmt::formatter<WFMath::Quaternion> : ostream_formatter {
};
template<>
struct fmt::formatter<WFMath::Vector<3>> : ostream_formatter {
};
template<>
struct fmt::formatter<WFMath::Point<3>> : ostream_formatter {
};

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::smart_static_cast;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

std::chrono::steady_clock::time_point Entity::currentTime;


Entity::Entity(std::string id, TypeInfo* ty) :
		m_type(ty),
		m_location(nullptr),
		m_id(std::move(id)),
		m_stamp(-1),
		m_visible(false),
		m_waitingForParentBind(false),
		m_angularMag(0),
		m_updateLevel(0),
		m_hasBBox(false),
		m_moving(false) {
	assert(!m_id.empty());
	m_orientation.identity();


	if (m_type) {
		m_type->PropertyChanges.connect(sigc::mem_fun(*this, &Entity::typeInfo_PropertyChanges));
	}
}

Entity::~Entity() {
	shutdown();
}

void Entity::handleOperation(const Atlas::Objects::Operation::RootOperation& op, TypeService& typeService) {
	OperationFrom.emit(op);

	switch (op->getClassNo()) {
		case Atlas::Objects::Operation::SIGHT_NO:
			for (auto& arg: op->getArgs()) {
				if (arg->instanceOf(Atlas::Objects::Entity::ROOT_ENTITY_NO)) {
					auto sightEntity = smart_dynamic_cast<RootEntity>(arg);
					if (sightEntity) {
						if (sightEntity->isDefaultId()) {
							logger->error("Got Sight.Entity op without inner id.");
						} else {
							if (sightEntity->getId() == getId()) {
								firstSight(sightEntity);
							} else {
								logger->error("Got Sight.Entity op with inner id of {} which doesn't matter the Sight:From value of {}.", sightEntity->getId(), op->getFrom());
							}
						}
					} else {
						logger->error("Got Sight.Entity without valid Entity as arg.");
					}

				} else if (arg->getClassNo() == SET_NO) {
					auto setOp = smart_dynamic_cast<Set>(arg);
					for (const auto& setArg: setOp->getArgs()) {
						auto setEntity = smart_dynamic_cast<RootEntity>(setArg);
						if (setEntity) {
							if (setEntity->isDefaultId()) {
								logger->error("Got Sight.Set op without inner id.");
							} else {
								if (setEntity->getId() == getId()) {
									setFromRoot(setEntity);
								} else {
									logger->error("Got Sight.Set op with inner id of {} which doesn't matter the Sight:From value of {}.", arg->getId(), op->getFrom());
								}
							}
						} else {
							logger->error("Got Sight.Set without valid Entity as arg.");
						}
					}
				} else if (arg->getClassNo() == HIT_NO) {
					//For hits we want to check the "to" field rather than the "from" field. We're more interested in
					//the entity that was hit than the one which did the hitting.
					if (!op->isDefaultTo()) {
						Entity* otherEntity = getEntity(op->getTo());
						if (otherEntity) {
							otherEntity->onHit(smart_dynamic_cast<Atlas::Objects::Operation::Hit>(arg));
						}
					} else {
						logger->warn("received 'hit' with TO unset");
					}
				} else {
					if (!arg->isDefaultParent()) {
						// we have to handle generic 'actions' late, to avoid trapping interesting
						// such as create or divide
						TypeInfo* ty = typeService.getTypeForAtlas(arg);
						if (!ty->isBound()) {
							m_typeDelayedOperations[ty].emplace_back(op, TypeDelayedOperation::Type::SIGHT);
						} else {
							if (ty->isA("action")) {
								if (op->isDefaultFrom()) {
									logger->warn("received op '{}' with FROM unset", ty->getName());
								} else {
									//We can be pretty sure that the arg is an Operation
									onAction(smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(arg), *ty);
								}
							}
						}
					}
				}
			}
			break;
		case Atlas::Objects::Operation::SOUND_NO:
			for (auto& arg: op->getArgs()) {
				if (arg->getClassNo() == TALK_NO) {
					onTalk(smart_dynamic_cast<Talk>(arg));
				} else {
					if (!arg->isDefaultParent()) {
						// we have to handle generic 'actions' late, to avoid trapping interesting
						// such as create or divide
						TypeInfo* ty = typeService.getTypeForAtlas(arg);
						if (!ty->isBound()) {
							m_typeDelayedOperations[ty].emplace_back(op, TypeDelayedOperation::Type::SIGHT);
						} else {
							if (ty->isA("communicate")) {
								if (op->isDefaultFrom()) {
									logger->warn("received op '{}' with FROM unset", ty->getName());
								} else {
									//We can be pretty sure that the arg is an Operation
									onSoundAction(smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(arg), *ty);
								}
							}
						}
					}
				}
			}
			break;
	}

}

void Entity::shutdown() {
	setLocation(nullptr);

	for (auto& child: m_contents) {
		//Release all children.
		child->setLocation(nullptr, false);
	}
	m_contents.clear();

	//Delete any lingering tasks.
	for (auto& entry: m_tasks) {
		TaskRemoved(entry.first, entry.second.get());
	}
}

void Entity::init(const RootEntity& ge, bool fromCreateOp) {
	// setup initial state
	firstSight(ge);
}


Entity* Entity::getTopEntity() {
	if (m_waitingForParentBind) {
		return nullptr;
	}
	if (!m_location) {
		return this;
	}
	return m_location->getTopEntity();
}

bool Entity::isAncestorTo(Eris::Entity& entity) const {
	if (!entity.getLocation()) {
		return false;
	}
	if (static_cast<const Eris::Entity*>(this) == entity.getLocation()) {
		return true;
	}
	return isAncestorTo(*entity.getLocation());

}

const Element& Entity::valueOfProperty(const std::string& name) const {
	///first check with the instance properties
	auto A = m_properties.find(name);
	if (A == m_properties.end()) {
		if (m_type) {
			///it wasn't locally defined, now check with typeinfo
			const Element* element = m_type->getProperty(name);
			if (element) {
				return *element;
			}
		}
		logger->error("did valueOfProperty({}) on entity {} which has no such name", name, m_id);
		throw InvalidOperation("no such property " + name);
	} else {
		return A->second;
	}
}

bool Entity::hasProperty(const std::string& p) const {
	///first check with the instance properties
	if (m_properties.find(p) != m_properties.end()) {
		return true;
	} else if (m_type) {
		///it wasn't locally defined, now check with typeinfo
		if (m_type->getProperty(p) != nullptr) {
			return true;
		}
	}
	return false;
}

const Element* Entity::ptrOfProperty(const std::string& name) const {
	///first check with the instance properties
	auto A = m_properties.find(name);
	if (A == m_properties.end()) {
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


Entity::PropertyMap Entity::getProperties() const {
	///Merge both the local properties and the type default properties.
	PropertyMap properties;
	properties.insert(m_properties.begin(), m_properties.end());
	if (m_type) {
		fillPropertiesFromType(properties, *m_type);
	}
	return properties;
}

const Entity::PropertyMap& Entity::getInstanceProperties() const {
	return m_properties;
}

void Entity::fillPropertiesFromType(Entity::PropertyMap& properties, const TypeInfo& typeInfo) const {
	properties.insert(typeInfo.getProperties().begin(), typeInfo.getProperties().end());
	///Make sure to fill from the closest properties first, as insert won't replace an existing value

	if (typeInfo.getParent()) {
		fillPropertiesFromType(properties, *typeInfo.getParent());
	}

}

sigc::connection Entity::observe(const std::string& propertyName, const PropertyChangedSlot& slot, bool evaluateNow) {
	// sometimes, I realize how great SigC++ is
	auto connection = m_observers[propertyName].connect(slot);
	if (evaluateNow) {
		auto prop = ptrOfProperty(propertyName);
		if (prop) {
			slot(*prop);
		}
	}
	return connection;
}

WFMath::Point<3> Entity::getViewPosition() const {
	WFMath::Point<3> vpos(0.0, 0.0, 0.0);
	//If the position is invalid, we will consider it to be (0,0,0) and skip applying it.
	for (const Entity* e = this; e; e = e->getLocation()) {
		if (e->getPosition().isValid()) {
			vpos = e->toLocationCoords(vpos);
		}
	}

	return vpos;
}

WFMath::Quaternion Entity::getViewOrientation() const {
	WFMath::Quaternion vor;

	vor.identity();
	for (const Entity* e = this; e; e = e->getLocation()) {
		if (e->getOrientation().isValid()) {
			vor *= e->getOrientation();
		}
	}

	return vor;
}

const WFMath::Point<3>& Entity::getPredictedPos() const {
	return m_predicted.position.value;
}

const WFMath::Quaternion& Entity::getPredictedOrientation() const {
	return m_predicted.orientation.value;
}

bool Entity::isMoving() const {
	return m_moving;
}

void Entity::updatePredictedState(const std::chrono::steady_clock::time_point& t, double simulationSpeed) {
	assert(isMoving());

	if (m_predicted.velocity.value.isValid() && m_predicted.velocity.value != WFMath::Vector<3>::ZERO()) {
		auto posDeltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(t - m_predicted.position.lastUpdated).count()) / 1'000'000.0;
		m_predicted.position.value = m_predicted.position.value + (m_predicted.velocity.value * posDeltaTime * simulationSpeed);
		m_predicted.position.lastUpdated = t;
	}
	if (m_angularVelocity.isValid() && m_angularMag != .0) {
		auto orientationDeltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(t - m_predicted.orientation.lastUpdated).count()) / 1'000'000.0;
		m_predicted.orientation.value = m_predicted.orientation.value * WFMath::Quaternion(m_angularVelocity, m_angularMag * orientationDeltaTime * simulationSpeed);
		m_predicted.orientation.lastUpdated = t;
	}
}

void Entity::firstSight(const RootEntity& gent) {
	if (!gent->isDefaultLoc()) {
		setLocationFromAtlas(gent->getLoc());
	} else {
		setLocation(nullptr);
	}

	setContentsFromAtlas(gent->getContains());
	//Since this is the first sight of this entity we should include all type props too.
	setFromRoot(gent, true);
}

void Entity::setFromRoot(const Root& obj, bool includeTypeInfoProperties) {
	beginUpdate();

	Atlas::Message::MapType properties;
	obj->addToMessage(properties);

	properties.erase("id"); //Id can't be changed once it's initially set, which it's at Entity creation time.
	properties.erase("contains"); //Contains are handled by the setContentsFromAtlas method which should be called separately.

	for (auto& entry: properties) {
		// see if the value in the sight matches the existing value
		auto I = m_properties.find(entry.first);
		if ((I != m_properties.end()) && (I->second == entry.second)) {
			continue;
		}
		try {
			setProperty(entry.first, entry.second);
		} catch (const std::exception& ex) {
			logger->warn("Error when setting property '{}'. Message: {}", entry.first, ex.what());
		}
	}

	//Add any values found in the type, if they aren't defined in the entity already.
	if (includeTypeInfoProperties && m_type) {
		Atlas::Message::MapType typeProperties;
		fillPropertiesFromType(typeProperties, *m_type);
		for (auto& entry: typeProperties) {
			propertyChangedFromTypeInfo(entry.first, entry.second);
		}
	}

	endUpdate();

}

void Entity::onTalk(const Atlas::Objects::Operation::RootOperation& talk) {
	const std::vector<Root>& talkArgs = talk->getArgs();
	if (talkArgs.empty()) {
		logger->warn("entity {} got sound(talk) with no args", getId());
		return;
	}

	for (const auto& arg: talkArgs) {
		Say.emit(arg);
	}
	//Noise.emit(talk);
}

void Entity::onLocationChanged(Entity* oldLoc) {
	LocationChanged.emit(oldLoc);
}


void Entity::onAction(const Atlas::Objects::Operation::RootOperation& arg, const TypeInfo& typeInfo) {
	Acted.emit(arg, typeInfo);
}

void Entity::onHit(const Atlas::Objects::Operation::Hit& arg) {
	Hit.emit(arg);
}

void Entity::onSoundAction(const Atlas::Objects::Operation::RootOperation& op, const TypeInfo& typeInfo) {
	Noise.emit(op, typeInfo);
}

void Entity::onImaginary(const Atlas::Objects::Root& arg) {
	Atlas::Message::Element attr;
	if (arg->copyAttr("description", attr) == 0 && attr.isString()) {
		Emote.emit(attr.asString());
	}
}

void Entity::setMoving(bool inMotion) {
	assert(m_moving != inMotion);

	m_moving = inMotion;
	Moving.emit(inMotion);

}

void Entity::onChildAdded(Entity* child) {
	ChildAdded.emit(child);
}

void Entity::onChildRemoved(Entity* child) {
	ChildRemoved(child);
}

void Entity::onTaskAdded(const std::string& id, Task* task) {
	TaskAdded(id, task);
}


void Entity::setProperty(const std::string& p, const Element& v) {
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

bool Entity::nativePropertyChanged(const std::string& p, const Element& v) {
	// in the future, hash these names to a compile-time integer index, and
	// make this a switch statement. The same index could also be used
	// in endUpdate

	if (p == "name") {
		m_name = v.asString();
		return true;
	} else if (p == "stamp") {
		m_stamp = std::chrono::milliseconds(v.asInt());
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
	} else if (p == "orientation") {
		m_orientation.fromAtlas(v);
		return true;
	} else if (p == "bbox") {
		m_bboxUnscaled.fromAtlas(v);
		m_bbox = m_bboxUnscaled;
		if (m_scale.isValid() && m_bbox.isValid()) {
			m_bbox.lowCorner().x() *= m_scale.x();
			m_bbox.lowCorner().y() *= m_scale.y();
			m_bbox.lowCorner().z() *= m_scale.z();
			m_bbox.highCorner().x() *= m_scale.x();
			m_bbox.highCorner().y() *= m_scale.y();
			m_bbox.highCorner().z() *= m_scale.z();
		}
		m_hasBBox = m_bbox.isValid();
		return true;
	} else if (p == "loc") {
		setLocationFromAtlas(v.asString());
		return true;
	} else if (p == "contains") {
		throw InvalidOperation("tried to set contains via setProperty");
	} else if (p == "tasks") {
		updateTasks(v);
		return true;
	} else if (p == "scale") {
		if (v.isList()) {
			if (v.List().size() == 1) {
				if (v.List().front().isNum()) {
					auto num = static_cast<WFMath::CoordType>(v.List().front().asNum());
					m_scale = WFMath::Vector<3>(num, num, num);
				}
			} else {
				m_scale.fromAtlas(v.List());
			}
		} else {
			m_scale = WFMath::Vector<3>();
		}
		m_bbox = m_bboxUnscaled;
		if (m_scale.isValid() && m_bbox.isValid()) {
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

void Entity::onPropertyChanged(const std::string& propertyName, const Element& v) {
	// no-op by default
}


void Entity::typeInfo_PropertyChanges(const std::string& propertyName, const Atlas::Message::Element& element) {
	propertyChangedFromTypeInfo(propertyName, element);
}

void Entity::propertyChangedFromTypeInfo(const std::string& propertyName, const Atlas::Message::Element& element) {
	///Only fire the events if there's no property already defined for this entity
	if (m_properties.find(propertyName) == m_properties.end()) {
		beginUpdate();
		nativePropertyChanged(propertyName, element);
		onPropertyChanged(propertyName, element);

		// fire observers

		auto obs = m_observers.find(propertyName);
		if (obs != m_observers.end()) {
			obs->second.emit(element);
		}

		addToUpdate(propertyName);
		endUpdate();
	}
}


void Entity::beginUpdate() {
	++m_updateLevel;
}

void Entity::addToUpdate(const std::string& propertyName) {
	assert(m_updateLevel > 0);
	m_modifiedProperties.insert(propertyName);
}

void Entity::endUpdate() {
	if (m_updateLevel < 1) {
		logger->error("mismatched begin/end update pair on entity");
		return;
	}

	if (--m_updateLevel == 0) // unlocking updates
	{
		Changed.emit(m_modifiedProperties);

		bool posChanged = m_modifiedProperties.find("pos") != m_modifiedProperties.end();
		bool orientationChanged = m_modifiedProperties.find("orientation") != m_modifiedProperties.end();
		bool velocityChanged = m_modifiedProperties.find("velocity") != m_modifiedProperties.end();

		if (posChanged ||
			velocityChanged ||
			orientationChanged ||
			m_modifiedProperties.find("angular") != m_modifiedProperties.end()) {
			auto now = currentTime;
			if (orientationChanged) {
				m_predicted.orientation.lastUpdated = now;
				m_predicted.orientation.value = m_orientation;
			}
			if (velocityChanged) {
				m_predicted.velocity.lastUpdated = now;
				m_predicted.velocity.value = m_velocity;
			}
			if (posChanged) {
				m_predicted.position.lastUpdated = now;
				m_predicted.position.value = m_position;
			}
			const WFMath::Vector<3>& velocity = getVelocity();
			bool nowMoving = (velocity.isValid() && (velocity.sqrMag() > 1e-3)) || (m_angularVelocity.isValid() && m_angularVelocity != WFMath::Vector<3>::ZERO());
			if (nowMoving != m_moving) {
				setMoving(nowMoving);
			}

			Moved.emit();
		}

		m_modifiedProperties.clear();
	}
}


void Entity::updateTasks(const Element& e) {
	if (e.isNone()) {
		for (auto& entry: m_tasks) {
			TaskRemoved(entry.first, entry.second.get());
		}
		m_tasks.clear();
		return;
	}
	if (!e.isMap()) {
		return; // malformed
	}
	auto& taskMap = e.Map();

	auto previousTasks = std::move(m_tasks);
	m_tasks.clear();

	for (auto& entry: taskMap) {
		auto& taskElement = entry.second;
		if (!taskElement.isMap()) {
			continue;
		}
		const MapType& tkmap(taskElement.Map());
		auto it = tkmap.find("name");
		if (it == tkmap.end()) {
			logger->error("task without name");
			continue;
		}
		if (!it->second.isString()) {
			logger->error("task with invalid name");
			continue;
		}

		auto tasksI = previousTasks.find(entry.first);
		std::unique_ptr<Task> task;

		bool newTask = false;
		if (tasksI == previousTasks.end()) {   // not found, create a new one
			task = std::make_unique<Task>(*this, it->second.asString());
			newTask = true;
		} else {
			task = std::move(tasksI->second);
			previousTasks.erase(entry.first);
		}

		task->updateFromAtlas(tkmap);
		if (newTask) {
			onTaskAdded(entry.first, task.get());
		}
		m_tasks.emplace(entry.first, std::move(task));
	} // of Atlas-specified tasks iteration

	for (auto& entry: previousTasks) {

		if (entry.second) {
			TaskRemoved(entry.first, entry.second.get());
		}
	} // of previous-task cleanup iteration
}

void Entity::setLocationFromAtlas(const std::string& locId) {
	if (locId.empty()) {
		return;
	}

	Entity* newLocation = getEntity(locId);
	if (!newLocation) {

		m_waitingForParentBind = true;
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

void Entity::setLocation(Entity* newLocation, bool removeFromOldLocation) {
	if (newLocation == m_location) return;

	if (newLocation) {
		m_waitingForParentBind = newLocation->m_waitingForParentBind;
	}

// do the actual member updating
	bool wasVisible = isVisible();
	if (m_location && removeFromOldLocation) {
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

void Entity::addToLocation() {
	assert(!m_location->hasChild(m_id));
	m_location->addChild(this);
}

void Entity::removeFromLocation() {
	assert(m_location->hasChild(m_id));
	m_location->removeChild(this);
}

void Entity::buildEntityDictFromContents(IdEntityMap& dict) {
	for (auto& child: m_contents) {
		dict[child->getId()] = child;
	}
}

void Entity::setContentsFromAtlas(const std::vector<std::string>& contents) {
// convert existing contents into a map, for fast membership tests
	IdEntityMap oldContents;
	buildEntityDictFromContents(oldContents);

// iterate over new contents
	for (auto& content: contents) {
		Entity* child;

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

			if (child->m_waitingForParentBind) {
				assert(!child->m_visible);
				child->m_waitingForParentBind = false;
			}

			/* we have found the child, update its location */
			child->setLocation(this);
		}

		child->setVisible(true);
	} // of contents list iteration

// mark previous contents which are not in new contents as invisible
	for (auto& entry: oldContents) {
		entry.second->setVisible(false);
	}
}

bool Entity::hasChild(const std::string& eid) const {
	for (auto& m_content: m_contents) {
		if (m_content->getId() == eid) {
			return true;
		}
	}

	return false;
}

void Entity::addChild(Entity* e) {
	m_contents.push_back(e);
	onChildAdded(e);
	assert(e->getLocation() == this);
}

void Entity::removeChild(Entity* e) {
	assert(e->getLocation() == this);

	auto I = std::find(m_contents.begin(), m_contents.end(), e);
	if (I != m_contents.end()) {
		m_contents.erase(I);
		onChildRemoved(e);
		return;
	}
	logger->error("child {} of entity {} not found doing remove", e->getId(), m_id);
}

// visiblity related methods

void Entity::setVisible(bool vis) {
	// force visibility to false if in limbo; necessary for the character entity,
	// which otherwise gets double appearances on activation
	if (m_waitingForParentBind) vis = false;

	bool wasVisible = isVisible(); // store before we update m_visible
	m_visible = vis;

	updateCalculatedVisibility(wasVisible);
}

bool Entity::isVisible() const {
	if (m_waitingForParentBind) return false;

	if (m_location) {
		return m_visible && m_location->isVisible();
	} else {
		return m_visible; // only for the root entity
	}
}

void Entity::updateCalculatedVisibility(bool wasVisible) {
	bool nowVisible = isVisible();
	if (nowVisible == wasVisible) return;

	/* the following code looks odd, so remember that only one of nowVisible and
	wasVisible can ever be true. The structure is necessary so that we fire
	Appearances top-down, but Disappearances bottom-up. */

	if (nowVisible) {
		onVisibilityChanged(true);
	}

	for (auto& item: m_contents) {
		/* in case this isn't clear; if we were visible, then child visibility
		was simply it's locally set value; if we were invisible, that the
		child must also have been invisible too. */
		bool childWasVisible = wasVisible && item->m_visible;
		item->updateCalculatedVisibility(childWasVisible);
	}

	if (wasVisible) {
		onVisibilityChanged(false);
	}
}

void Entity::onVisibilityChanged(bool vis) {
	VisibilityChanged.emit(vis);
}

std::optional<std::string> Entity::extractEntityId(const Atlas::Message::Element& element) {
	if (element.isString()) {
		return element.String();
	} else if (element.isMap()) {
		auto I = element.asMap().find("$eid");
		if (I != element.asMap().end() && I->second.isString()) {
			return I->second.String();
		}
	}
	return {};

}

void Entity::typeBound(TypeInfo* type) {
	auto I = m_typeDelayedOperations.find(type);
	if (I != m_typeDelayedOperations.end()) {
		for (const auto& entry: I->second) {
			if (entry.type == TypeDelayedOperation::Type::SIGHT) {
				onAction(entry.operation, *type);
			} else {
				onSoundAction(entry.operation, *type);
			}
		}
		m_typeDelayedOperations.erase(I);
	}
}

void Entity::typeBad(TypeInfo* type) {
	auto I = m_typeDelayedOperations.find(type);
	if (I != m_typeDelayedOperations.end()) {
		logger->warn("The type '{}' couldn't be bound and we had to discard {} ops sent to the entity {}.",
					 type->getName(),
					 I->second.size(),
					 getId());
		m_typeDelayedOperations.erase(I);
	} else {
		logger->warn("The type '{}' couldn't be bound.", type->getName());
	}
}


} // of namespace
