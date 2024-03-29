/*
 Copyright (C) 2004  Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "EmberEntity.h"

#include "IGraphicalRepresentation.h"
#include "IEntityAttachment.h"
#include "IEntityVisitor.h"
#include "IHeightProvider.h"
#include "framework/ConsoleBackend.h"
#include "framework/Log.h"
#include "framework/AtlasQuery.h"

#include <Eris/TypeInfo.h>
#include <Eris/View.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/MultiLineListFormatter.h>
#include <fmt/ostream.h>
#include <sstream>
#include <iostream>
#include <utility>

template<>
struct fmt::formatter<Eris::Entity> : ostream_formatter {
};
template<>
struct fmt::formatter<Ember::EmberEntity> : ostream_formatter {
};

namespace Ember {


/**
 * @brief Handles dispatching of attribute changes to global listeners.
 *
 * This is useful for any listeners that want to observe changes to specific attribute, independent of on which entity it's placed.
 */
class GlobalAttributeDispatcher {
private:
	std::unordered_multimap<std::string, std::function<void(EmberEntity&, const Atlas::Message::Element&)>*> mCallbacks;
public:

	~GlobalAttributeDispatcher() {
		//Make sure that all callbacks are de-registered.
		assert(mCallbacks.empty());
	}

	void registerListener(const std::string& attributeName, std::function<void(EmberEntity&, const Atlas::Message::Element&)>& listener) {
		mCallbacks.insert(std::make_pair(attributeName, &listener));
	}

	void deregisterListener(const std::string& attributeName, std::function<void(EmberEntity&, const Atlas::Message::Element&)>& listener) {
		for (auto range = mCallbacks.equal_range(attributeName); range.first != range.second; ++range.first) {
			if (range.first->second == &listener) {
				mCallbacks.erase(range.first);
				break;
			}
		}
	}

	void dispatchAttributeChange(EmberEntity& entity, const std::string& attributeName, const Atlas::Message::Element& value) {
		for (auto range = mCallbacks.equal_range(attributeName); range.first != range.second; ++range.first) {
			(*range.first->second)(entity, value);
		}
	}
};

GlobalAttributeDispatcher sGlobalDispatcher;

EmberEntity::EmberEntity(std::string id, Eris::TypeInfo* ty, Eris::Entity::EntityContext context) :
		Eris::Entity(std::move(id), ty, std::move(context)),
		mPositioningMode(PositioningMode::FREE),
		mAttachment(nullptr),
		mAttachmentControlDelegate(nullptr),
		mHeightProvider(nullptr) {
}

EmberEntity::~EmberEntity() {
	shutdown();
}

void EmberEntity::registerGlobalAttributeListener(const std::string& attributeName, std::function<void(EmberEntity&, const Atlas::Message::Element&)>& listener) {
	sGlobalDispatcher.registerListener(attributeName, listener);
}

void EmberEntity::deregisterGlobalAttributeListener(const std::string& attributeName, std::function<void(EmberEntity&, const Atlas::Message::Element&)>& listener) {
	sGlobalDispatcher.deregisterListener(attributeName, listener);
}


std::string EmberEntity::getNameOrType() const {
	if (!m_name.empty()) {
		return m_name;
	}
	return m_type->getName();
}

void EmberEntity::adjustPosition() {
	if (mAttachment) {
		mAttachment->updatePosition();
	}
}

void EmberEntity::setHeightProvider(IHeightProvider* heightProvider) {
	mHeightProvider = heightProvider;
}

double EmberEntity::getHeight(const WFMath::Point<2>& localPosition) const {

	if (mHeightProvider) {
		float height = 0;
		if (mHeightProvider->getHeight(WFMath::Point<2>(localPosition.x(), localPosition.y()), height)) {
			return height;
		}
	}

	//A normal EmberEntity shouldn't know anything about the terrain, so we can't handle the area here.
	//Instead we just pass it on to the parent until we get to someone who knows how to handle this (preferably the terrain).
	if (getEmberLocation()) {

		WFMath::Point<2> adjustedLocalPosition(getPredictedPos().x(), getPredictedPos().z());

		WFMath::Vector<3> xVec = WFMath::Vector<3>(1.0, 0.0, 0.0).rotate(getOrientation());
		auto theta = std::atan2(xVec.z(), xVec.x()); // rotation about Y
		WFMath::RotMatrix<2> rm;
		WFMath::Vector<2> adjustment(localPosition.x(), localPosition.y());
		adjustment.rotate(rm.rotation(theta));
		adjustedLocalPosition += adjustment;

		return getEmberLocation()->getHeight(adjustedLocalPosition) - getPredictedPos().y();
	}

	WFMath::Point<3> predictedPos = getPredictedPos();
	if (predictedPos.isValid()) {
		return predictedPos.y();
	} else {
		return 0.0f;
	}
}

void EmberEntity::setAttachmentControlDelegate(IEntityControlDelegate* delegate) {
	mAttachmentControlDelegate = delegate;
	if (mAttachment) {
		mAttachment->setControlDelegate(delegate);
	}
}

IEntityControlDelegate* EmberEntity::getAttachmentControlDelegate() const {
	return mAttachmentControlDelegate;
}

void EmberEntity::onLocationChanged(Eris::Entity* oldLocation) {
	auto emberEntityOldLocation = dynamic_cast<EmberEntity*>(oldLocation);
	if (emberEntityOldLocation && emberEntityOldLocation->getAttachment()) {
		emberEntityOldLocation->getAttachment()->detachEntity(*this);
	}
	updateAttachment();

	Eris::Entity::onLocationChanged(oldLocation);

}

void EmberEntity::updateAttachment() {

	//Get the new location. We use getEmberLocation() since we always know that all entities are of type EmberEntity.
	EmberEntity* newLocationEntity = getEmberLocation();

	if (newLocationEntity && newLocationEntity->getAttachment()) {
		try {
			mAttachment.reset();
			newLocationEntity->getAttachment()->attachEntity(*this);

		} catch (const std::exception& ex) {
			logger->warn("Problem when creating new attachment for entity. {}", ex.what());
		}
	} else {
		try {
			setAttachment(nullptr);
		} catch (const std::exception& ex) {
			logger->warn("Problem when setting attachment for entity. {}", ex.what());
		}
	}
}

void EmberEntity::onPropertyChanged(const std::string& str, const Atlas::Message::Element& v) {
	if (str == "mode" && v.isString()) {
		parsePositioningModeChange(v.String());
	} else if (str == "bbox" || str == "scale") {
		Entity::onPropertyChanged(str, v);
		onBboxChanged();
		return;
	} else if (str == "usages") {
		parseUsages(mUsages, v);
	} else if (str == "_usages") {
		parseUsages(mUsagesProtected, v);
	} else if (str == "actions") {
		auto changes = processActionsChange(v);
		if (!changes.empty()) {
			EventActionsChanged.emit(changes);
		}
	}

	//Dispatch attribute changes to any global listeners.
	sGlobalDispatcher.dispatchAttributeChange(*this, str, v);

	Entity::onPropertyChanged(str, v);

}

std::vector<ActionChange> EmberEntity::processActionsChange(const Atlas::Message::Element& v) {
	std::vector<ActionChange> actionChanges;
	if (!v.isMap()) {
		//Remove all existing actions
		for (auto& entry: mActionsData) {
			actionChanges.emplace_back(ActionChange::ChangeType::Removed, ActionEntry{entry.first, std::chrono::milliseconds(0), std::chrono::milliseconds(0)});
		}
		mActionsData.clear();
	} else if (mActionsData.empty()) {
		auto& newMap = v.Map();
		//All new actions
		for (auto& newEntry: newMap) {
			AtlasQuery::find<Atlas::Message::IntType>(newEntry.second, "start_time", [&](const auto& startTime) {
				ActionEntry actionEntry{newEntry.first, std::chrono::milliseconds(startTime), std::chrono::milliseconds(0)};
				actionChanges.emplace_back(ActionChange::ChangeType::Added, std::move(actionEntry));
			});
		}
		mActionsData = newMap;
	} else {
		auto& newMap = v.Map();
		auto existingMap = mActionsData;
		for (auto& newEntry: newMap) {
			auto existingI = existingMap.find(newEntry.first);
			if (existingI == existingMap.end()) {
				AtlasQuery::find<Atlas::Message::IntType>(newEntry.second, "start_time", [&](const auto& startTime) {
					ActionEntry actionEntry{newEntry.first, std::chrono::milliseconds(startTime), std::chrono::milliseconds(0)};
					actionChanges.emplace_back(ActionChange::ChangeType::Added, std::move(actionEntry));
				});
			} else {
				AtlasQuery::find<Atlas::Message::IntType>(newEntry.second, "start_time", [&](const auto& startTime) {
					ActionEntry actionEntry{newEntry.first, std::chrono::milliseconds(startTime), std::chrono::milliseconds(0)};
					actionChanges.emplace_back(ActionChange::ChangeType::Updated, std::move(actionEntry));
				});
				existingMap.erase(existingI);
			}
		}
		for (auto& entry: existingMap) {
			actionChanges.emplace_back(ActionChange::ChangeType::Removed, ActionEntry{entry.first, std::chrono::milliseconds(0), std::chrono::milliseconds(0)});
		}
		mActionsData = newMap;
	}

	return actionChanges;

}


void EmberEntity::parsePositioningModeChange(const std::string& mode) {
	PositioningMode newMode;
	if (mode == "floating") {
		newMode = PositioningMode::FLOATING;
	} else if (mode == "fixed") {
		newMode = PositioningMode::FIXED;
	} else if (mode == "submerged") {
		newMode = PositioningMode::SUBMERGED;
	} else if (mode == "planted") {
		newMode = PositioningMode::PLANTED;
	} else if (mode == "projectile") {
		newMode = PositioningMode::PROJECTILE;
	} else {
		newMode = PositioningMode::FREE;
	}

	if (newMode != mPositioningMode) {
		onPositioningModeChanged(newMode);
	}
}

void EmberEntity::onPositioningModeChanged(PositioningMode newMode) {
	adjustPosition();

	EventPositioningModeChanged.emit(newMode);
	mPositioningMode = newMode;
}

void EmberEntity::setVisualize(const std::string& visualization, bool visualize) {
	if (mAttachment) {
		mAttachment->setVisualize(visualization, visualize);
	}
}

bool EmberEntity::getVisualize(const std::string& visualization) const {
	if (mAttachment) {
		return mAttachment->getVisualize(visualization);
	}
	return false;
}

void EmberEntity::onBboxChanged() {
	if (mAttachment) {
		mAttachment->updateScale();
	}
}

EmberEntity* EmberEntity::getEmberLocation() const {
	return dynamic_cast<EmberEntity*>(getLocation());
}

EmberEntity* EmberEntity::getEmberContained(size_t index) const {
	return dynamic_cast<EmberEntity*>(getContained(index));
}

void EmberEntity::dumpAttributes(std::iostream& outstream) const {

	Atlas::Message::QueuedDecoder decoder;

	Atlas::Codecs::XML codec(outstream, outstream, decoder);
	Atlas::MultiLineListFormatter formatter(outstream, codec);
	Atlas::Message::Encoder encoder(formatter);
	formatter.streamBegin();
	encoder.streamMessageElement(getProperties());

	formatter.streamEnd();
}

IGraphicalRepresentation* EmberEntity::getGraphicalRepresentation() const {
	if (mAttachment) {
		return mAttachment->getGraphicalRepresentation();
	}
	return nullptr;
}

void EmberEntity::setAttachment(std::unique_ptr<IEntityAttachment> attachment) {
	//Make sure that the old attachment is destroyed after "reattachedChildren" is called, since they might call into it when detaching.
	auto oldAttachment = std::move(mAttachment);
	mAttachment = std::move(attachment);
	EventChangedGraphicalRepresentation();
	EventAttachmentChanged();
	reattachChildren();
}

void EmberEntity::reattachChildren() {
	for (size_t i = 0; i < numContained(); ++i) {
		EmberEntity* entity = getEmberContained(i);
		if (entity) {
			entity->updateAttachment();
			entity->reattachChildren();
		}
	}
}

const std::unique_ptr<IEntityAttachment>& EmberEntity::getAttachment() const {
	return mAttachment;
}

EmberEntity* EmberEntity::getAttachedEntity(const std::string& attachment) {
	auto* attachmentElement = ptrOfProperty(attachment);
	if (attachmentElement) {
		auto id = Eris::Entity::extractEntityId(*attachmentElement);
		if (id) {
			for (size_t i = 0; i < numContained(); ++i) {
				EmberEntity* entity = getEmberContained(i);
				if (entity && entity->getId() == *id) {
					return entity;
				}
			}
		}
	}

	return nullptr;
}

void EmberEntity::accept(IEntityVisitor& visitor) {
	visitor.visit(*this);
	for (size_t i = 0; i < numContained(); ++i) {
		EmberEntity* entity = getEmberContained(i);
		if (entity) {
			entity->accept(visitor);
		}
	}
}

void EmberEntity::accept(std::function<bool(EmberEntity&)>& visitor) {
	if (visitor(*this)) {
		for (size_t i = 0; i < numContained(); ++i) {
			EmberEntity* entity = getEmberContained(i);
			if (entity) {
				entity->accept(visitor);
			}
		}
	}
}

void EmberEntity::accept(std::function<bool(const EmberEntity&)>& visitor) const {
	if (visitor(*this)) {
		for (size_t i = 0; i < numContained(); ++i) {
			EmberEntity* entity = getEmberContained(i);
			if (entity) {
				entity->accept(visitor);
			}
		}
	}
}

void EmberEntity::parseUsages(std::map<std::string, Eris::Usage>& map, const Atlas::Message::Element& element) {
	map.clear();

	if (element.isMap()) {
		for (auto& entry: element.Map()) {
			Eris::Usage usage;
			AtlasQuery::find<Atlas::Message::StringType>(entry.second, "constraint", [&](const auto& constraint) {
				usage.constraint = constraint;
			});
			AtlasQuery::find<Atlas::Message::StringType>(entry.second, "description", [&](const auto& description) {
				usage.description = description;
			});
			AtlasQuery::find<Atlas::Message::MapType>(entry.second, "params", [&](const auto& params) {
				for (auto& paramEntry: params) {
					Eris::UsageParameter usageParam;
					AtlasQuery::find<Atlas::Message::StringType>(paramEntry.second, "type", [&](const auto& type) {
						usageParam.type = type;
					});
					AtlasQuery::find<Atlas::Message::IntType>(paramEntry.second, "min", [&](const auto& min) {
						usageParam.min = min;
					});
					AtlasQuery::find<Atlas::Message::IntType>(paramEntry.second, "max", [&](const auto& max) {
						usageParam.max = max;
					});
					AtlasQuery::find<Atlas::Message::StringType>(paramEntry.second, "constraint", [&](const auto& constraint) {
						usageParam.constraint = constraint;
					});
					usage.params.emplace(paramEntry.first, std::move(usageParam));
				}
			});
			usage.name = entry.first;
			AtlasQuery::find<Atlas::Message::StringType>(entry.second, "name", [&](const auto& name) {
				usage.name = name;
			});

			map.emplace(entry.first, std::move(usage));
		}
	}
}

const std::map<std::string, Eris::Usage>& EmberEntity::getUsages() const {
	return mUsages;
}

const std::map<std::string, Eris::Usage>& EmberEntity::getUsagesProtected() const {
	return mUsagesProtected;
}

std::string EmberEntity::describeEntity() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

}

namespace Eris {

std::ostream& operator<<(std::ostream& s, const Eris::Entity& entity) {
	auto& name = entity.getName();
	s << entity.getId();
	if (entity.getType()) {
		s << "(" << entity.getType()->getName();
		if (!name.empty()) {
			s << ",'" << name << "'";
		}
		s << ")";
	} else {
		if (!name.empty()) {
			s << "('" << name << "')";
		}
	}
	return s;
}

}
