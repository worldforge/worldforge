/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

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

#include "ModelRepresentation.h"

#include "Model.h"
#include "SubModel.h"
#include "ParticleSystemBinding.h"
#include "ModelMount.h"
#include "ModelPartReactivatorVisitor.h"

#include "EmberEntityMappingManager.h"

#include "components/ogre/sound/SoundEntity.h"

#include "components/ogre/MousePicker.h"
#include "components/ogre/MotionManager.h"
#include "components/ogre/Scene.h"
#include "components/ogre/Convert.h"
#include "components/ogre/EntityCollisionInfo.h"
#include "services/sound/SoundService.h"

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreParticleSystem.h>
#include "domain/EntityTalk.h"

#include <Eris/Task.h>
#include "framework/IResourceProvider.h"
#include "services/sound/SoundSample.h"


namespace Ember::OgreView::Model {

std::string ModelRepresentation::sTypeName("ModelRepresentation");

const char* const ModelRepresentation::ACTION_STAND("idle");

const char* const ModelRepresentation::ACTION_RUN("run");
const char* const ModelRepresentation::ACTION_RUN_RIGHT("run_right");
const char* const ModelRepresentation::ACTION_RUN_LEFT("run_left");
const char* const ModelRepresentation::ACTION_RUN_BACKWARDS("run_backwards");

const char* const ModelRepresentation::ACTION_WALK("walk");
const char* const ModelRepresentation::ACTION_WALK_RIGHT("walk_right");
const char* const ModelRepresentation::ACTION_WALK_LEFT("walk_left");
const char* const ModelRepresentation::ACTION_WALK_BACKWARDS("walk_backwards");

const char* const ModelRepresentation::ACTION_SWIM("swim");
const char* const ModelRepresentation::ACTION_TREAD_WATER("tread_water");

const char* const ModelRepresentation::ACTION_FLOAT("float");

ModelRepresentation::ModelRepresentation(EmberEntity& entity, std::unique_ptr<Model> model, Scene& scene, EntityMapping::EntityMapping& mapping) :
		mEntity(entity),
		mModel(std::move(model)),
		mScene(scene),
		mMapping(mapping),
		mCurrentMovementAction(nullptr),
		mActiveAction(nullptr),
		mUserObject(std::make_shared<EmberEntityUserObject>(EmberEntityUserObject{entity})),
		mBulletCollisionDetector(std::make_unique<BulletCollisionDetector>(scene.getBulletWorld())) {
	mBulletCollisionDetector->collisionInfo = EntityCollisionInfo{&entity, false};
	//Only connect if we have actions to act on
	if (!mModel->getDefinition()->getActionDefinitions().empty()) {
		mEntity.EventActionsChanged.connect(sigc::mem_fun(*this, &ModelRepresentation::entity_ActionsChanged));
		mEntity.OperationFrom.connect(sigc::mem_fun(*this, &ModelRepresentation::entity_Operation));
	}
	//Only connect if we have particles
	if (mModel->hasParticles()) {
		mEntity.Changed.connect(sigc::mem_fun(*this, &ModelRepresentation::entity_Changed));
	}

	//listen for reload or reset events from the model. This allows us to alter model definitions at run time and have the in game entities update.
	mModel->Reloaded.connect(sigc::mem_fun(*this, &ModelRepresentation::model_Reloaded));
	mModel->Resetting.connect(sigc::mem_fun(*this, &ModelRepresentation::model_Resetting));
	mEntity.EventPositioningModeChanged.connect(sigc::mem_fun(*this, &ModelRepresentation::entity_PositioningModeChanged));

	mModel->setQueryFlags(MousePicker::CM_ENTITY);

	parseMovementMode(mEntity.getVelocity());

	mModel->setUserObject(mUserObject);

	updateCollisionDetection();

	//start out with the default movement mode
	//onMovementModeChanged(ModelRepresentation::MM_DEFAULT);

}

ModelRepresentation::~ModelRepresentation() {

	const RenderingDefinition* renderingDef = mModel->getDefinition()->getRenderingDefinition();
	if (renderingDef && !renderingDef->scheme.empty()) {
		mScene.deregisterEntityWithTechnique(mEntity, renderingDef->scheme);
	}
	if (mEntity.getPositioningMode() == EmberEntity::PositioningMode::PROJECTILE) {
		mScene.deregisterEntityWithTechnique(mEntity, "projectile");
	}

	if (mCurrentMovementSound) {
		mCurrentMovementSound->stop();
	}

	//mModel->_getManager()->destroyMovableObject(&mModel);

	//make sure it's not in the MotionManager
	//TODO: keep a marker in the entity so we don't need to call this for all entities
	MotionManager::getSingleton().removeAnimated(mEntity.getId());

}

const std::string& ModelRepresentation::getType() const {
	return sTypeName;
}

const std::string& ModelRepresentation::getTypeNameForClass() {
	return sTypeName;
}

EmberEntity& ModelRepresentation::getEntity() const {
	return mEntity;
}

Model& ModelRepresentation::getModel() const {
	return *mModel;
}

void ModelRepresentation::setModelPartShown(const std::string& partName, bool visible) {
	if (mModel->isLoaded()) {

		if (visible) {
			mModel->showPart(partName);
		} else {
			mModel->hidePart(partName);
		}
	}
}

void ModelRepresentation::initFromModel() {
	connectEntities();

	//see if we should use a rendering technique different from the default one (which is just using the Model::Model instance)
	const RenderingDefinition* renderingDef = mModel->getDefinition()->getRenderingDefinition();
	if (renderingDef && !renderingDef->scheme.empty() && mModel->isLoaded()) {
		mScene.registerEntityWithTechnique(mEntity, renderingDef->scheme);
//		Environment::Forest* forest = EmberOgre::getSingleton().getEntityFactory()->getWorld()->getEnvironment()->getForest();
//		forest->addEmberEntity(this);
	}

	if (mEntity.getPositioningMode() == EmberEntity::PositioningMode::PROJECTILE) {
		mScene.registerEntityWithTechnique(mEntity, "projectile");
	}

	/** If there's an idle animation, we'll randomize the entry value for that so we don't end up with too many similar
	 * entities with synchronized animations (such as when you enter the world at origo and have 20 settlers doing the exact same motions. */
	auto idleAction = mModel->getAction(ActivationDefinition::Type::MOVEMENT, ACTION_STAND);
	if (idleAction) {
		idleAction->animations.addTime(Ogre::Math::RangeRandom(0, 15));
	}

	//If there are particles, update the bindings.
	if (mModel->hasParticles()) {
		auto& bindings = mModel->getAllParticleSystemBindings();
		for (auto& binding: bindings) {
			auto elemPtr = mEntity.ptrOfProperty(binding.mVariableName);
			if (elemPtr && elemPtr->isNum()) {
				binding.scaleValue(static_cast<Ogre::Real>(elemPtr->asNum()));
			}
		}
	}


}

Ogre::Vector3 ModelRepresentation::getScale() const {
	return mModel->getScale();
}

void ModelRepresentation::connectEntities() {
}

void ModelRepresentation::model_Reloaded() {
	initFromModel();
	reactivatePartActions();
	//Retrigger a movement change so that animations can be stopped and started now that the model has changed.
	parseMovementMode(mEntity.getVelocity());
	updateCollisionDetection();
	//See if we have any pending ops that we need to check for actions.
	for (const auto& op: mPendingEntityOperations) {
		processEntityOperation(op);
	}
	mPendingEntityOperations.clear();
	//And also see if we should activate any actions again.
	auto actionsProp = mEntity.ptrOfProperty("actions");
	if (actionsProp && actionsProp->isMap()) {
		for (const auto& entry: actionsProp->Map()) {
			processAddedAction(entry.first);
		}
	}
}

void ModelRepresentation::model_Resetting() {
	//Resetting will invalidate all actions, so set them to null here.
	if (mCurrentMovementAction || mActiveAction) {
		MotionManager::getSingleton().removeAnimated(mEntity.getId());
	}
	mActiveAction = nullptr;
	mCurrentMovementAction = nullptr;
}

void ModelRepresentation::entity_Changed(const std::set<std::string>& attributeIds) {
	for (const auto& attributeId: attributeIds) {
		attrChanged(attributeId, mEntity.valueOfProperty(attributeId));
	}
}

void ModelRepresentation::attrChanged(const std::string& str, const Atlas::Message::Element& v) {

	//check if the changed attribute should affect any particle systems
	//TODO: refactor this into a system where the Model instead keeps track of whether any particle systems are in use and if so attaches listeners.
	if (mModel->hasParticles()) {
		auto& bindings = mModel->getAllParticleSystemBindings();
		for (auto& binding: bindings) {
			if (binding.mVariableName == str && v.isNum()) {
				binding.scaleValue(static_cast<Ogre::Real>(v.asNum()));
			}
		}
	}

}

Action* ModelRepresentation::getActionForMovement(const WFMath::Vector<3>& velocity) const {
	double mag = 0;
	if (velocity.isValid()) {
		mag = velocity.mag();
	}

	if (mEntity.getPositioningMode() == EmberEntity::PositioningMode::SUBMERGED || mEntity.getPositioningMode() == EmberEntity::PositioningMode::FLOATING) {
		if (mag < 0.01) {
			return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_TREAD_WATER, ACTION_SWIM, ACTION_WALK});
		} else {
			return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_SWIM, ACTION_TREAD_WATER, ACTION_WALK});
		}
	} else {

		if (mag < 0.01) {
			return mModel->getAction(ActivationDefinition::Type::MOVEMENT, ACTION_STAND);
		} else {

			//The model is moving in some direction; we need to figure out both the direction, and the speed.
			//We'll split up the movement into four arcs: forwards, backwards, left and right
			//We'll use a little bit of padding, so that the side movement arcs are larger.
			bool isRunning = mag > 2.6;
			WFMath::CoordType atan = std::atan2(velocity.x(), velocity.z());

			if (atan <= 0.7 && atan >= -0.7) {
				//moving forwards
				if (isRunning) {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_RUN, ACTION_WALK});
				} else {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_WALK, ACTION_RUN});
				}
			} else if (atan >= 2.4 || atan <= -2.4) {
				//moving backwards
				if (isRunning) {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_RUN_BACKWARDS, ACTION_WALK_BACKWARDS});
				} else {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_WALK_BACKWARDS, ACTION_RUN_BACKWARDS});
				}
			} else if (atan > 0.7) {
				//moving to the left
				if (isRunning) {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_RUN_LEFT, ACTION_WALK_LEFT, ACTION_RUN, ACTION_WALK});
				} else {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_WALK_LEFT, ACTION_RUN_LEFT, ACTION_WALK, ACTION_RUN});
				}
			} else {
				//moving to the right
				if (isRunning) {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_RUN_RIGHT, ACTION_WALK_RIGHT, ACTION_RUN, ACTION_WALK});
				} else {
					return getFirstAvailableAction(ActivationDefinition::Type::MOVEMENT, {ACTION_WALK_RIGHT, ACTION_RUN_RIGHT, ACTION_WALK, ACTION_RUN});
				}
			}
		}
	}

}

Action* ModelRepresentation::getFirstAvailableAction(ActivationDefinition::Type type, std::initializer_list<const char* const> actions) const {
	for (auto& actionName: actions) {
		Action* action = mModel->getAction(type, actionName);
		if (action) {
			return action;
		}
	}
	return nullptr;
}

void ModelRepresentation::parseMovementMode(const WFMath::Vector<3>& velocity) {

	auto* newAction = getActionForMovement(velocity);

	if (newAction != mCurrentMovementAction) {
		//first disable any current action
		if (mCurrentMovementAction) {
			resetAnimations();
		}

		mCurrentMovementAction = newAction;
		if (newAction) {
			MotionManager::getSingleton().addAnimated(mEntity.getId(), this);
			playSoundForMovementAction(newAction);
		} else {
			MotionManager::getSingleton().removeAnimated(mEntity.getId());
		}
	}

}

void ModelRepresentation::setLocalVelocity(const WFMath::Vector<3>& velocity) {
	parseMovementMode(velocity);
}

void ModelRepresentation::updateAnimation(float timeSlice) {
	//This is a bit convoluted, but the logic is as follows:
	//If we're moving, i.e. with a non-zero velocity, we should always prefer to show the movement animation
	//If not, we should prefer to show a current action animation if available
	//If not, we should show any available task animation.
	//And if none of these applies, we should play the current movement action (which should be idle).

	const WFMath::Vector<3>& velocity = mEntity.getVelocity();
	if (mCurrentMovementAction && velocity.isValid() && velocity.mag() > 0.01f) {
		bool continuePlay = false;
		mCurrentMovementAction->animations.addTime(timeSlice, continuePlay);
	} else if (mActiveAction) {
		bool continuePlay = false;
		mActiveAction->animations.addTime(timeSlice, continuePlay);
//		if (!continuePlay) {
//			mActiveAction->animations.reset();
//			mActiveAction = nullptr;
//		}
	} else if (mCurrentMovementAction) {
		bool continuePlay = false;
		mCurrentMovementAction->animations.addTime(timeSlice, continuePlay);
	}

	if (mCurrentMovementSound) {
		if (mEntity.getPredictedPos().isValid()) {
			mCurrentMovementSound->setPosition(mEntity.getViewPosition());
		}
		if (mEntity.getVelocity().isValid()) {
			mCurrentMovementSound->setVelocity(mEntity.getVelocity());
		}
	}
}

void ModelRepresentation::resetAnimations() {
	if (mCurrentMovementAction) {
		mCurrentMovementAction->animations.reset();
	}
	if (mActiveAction) {
		mActiveAction->animations.reset();
	}
}

Action* ModelRepresentation::getActionForOperation(const Atlas::Objects::Operation::RootOperation& op) const {
	for (const auto& mapping: mModel->getDefinition()->getOperationsToActionMappings()) {
		auto compareOp = op;
		bool keepOn = true;
		auto I = mapping.operationsMatch.operationMatches.begin();
		while (keepOn && I != mapping.operationsMatch.operationMatches.end()) {
			if (*I != compareOp->getParent()) {
				keepOn = false;
				break;
			}
			I++;
			if (I != mapping.operationsMatch.operationMatches.end()) {
				if (compareOp->getArgs().empty()) {
					keepOn = false;
					break;
				} else {
					auto childOp = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(compareOp->getArgs().front());
					if (childOp) {
						compareOp = childOp;
					} else {
						keepOn = false;
						break;
					}
				}
			}
		}

		if (keepOn) {
			//So far there's a match for the operations. Check if we also need to check for any contained Entity attributes
			if (!mapping.operationsMatch.entityAttributeMatches.empty()) {
				auto entity = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Entity::RootEntity>(compareOp->getArgs().front());

				for (auto& attrMatcher: mapping.operationsMatch.entityAttributeMatches) {
					Atlas::Message::Element attrValue;
					if (entity->copyAttr(attrMatcher.name, attrValue) == 1) {
						keepOn = false;
						break;
					}
					if (attrValue != attrMatcher.value) {
						keepOn = false;
						break;
					}
				}


			}
			if (keepOn) {
				return mModel->getAction(mapping.actionName);
			}
		}
	}
	return nullptr;
}

void ModelRepresentation::entity_Operation(const Atlas::Objects::Operation::RootOperation& op) {
	if (!mModel->isLoaded()) {
		mPendingEntityOperations.emplace_back(op);
	} else {
		processEntityOperation(op);
	}
}

Action* ModelRepresentation::processEntityOperation(const Atlas::Objects::Operation::RootOperation& op) {
	auto action = getActionForOperation(op);
	if (action) {
		MotionManager::getSingleton().addAnimated(mEntity.getId(), this);
		mActiveAction = action;
		resetAnimations();
		playSoundForAction(action->soundAction);
	}
	return action;
}


Action* ModelRepresentation::processActivation(ActivationDefinition::Type type, const std::string& activityName) {
	Action* newAction = mModel->getAction(type, activityName);
	if (newAction) {
		MotionManager::getSingleton().addAnimated(mEntity.getId(), this);
		mActiveAction = newAction;
		resetAnimations();
		playSoundForAction(newAction->soundAction);
		return newAction;
	}
	return nullptr;
}

void ModelRepresentation::entity_ActionsChanged(const std::vector<ActionChange>& actionChanges) {
	if (mModel->isLoaded()) {
		for (auto& change: actionChanges) {
			if (change.changeType == ActionChange::ChangeType::Removed) {
				if (mActiveAction) {
					mActiveAction->animations.reset();
					mActiveAction = nullptr;
				}
			}
		}
		for (auto& change: actionChanges) {
			if (change.changeType == ActionChange::ChangeType::Added) {
				//TODO: handle transitions
				if (mActiveAction) {
					continue;
				}
				processAddedAction(change.entry.actionName);
			}
		}
	}
}

void ModelRepresentation::processAddedAction(const std::string& actionName) {
	auto result = processActivation(ActivationDefinition::Type::ACTION, actionName);

	//If there's no action found, try to see if we have a "default action" defined to play instead.
	if (!result) {
		processActivation(ActivationDefinition::Type::ACTION, "default_action");
	}

}

void ModelRepresentation::entity_PositioningModeChanged(EmberEntity::PositioningMode newMode) {
	if (newMode == EmberEntity::PositioningMode::PROJECTILE) {
		mScene.registerEntityWithTechnique(mEntity, "projectile");
	} else if (mEntity.getPositioningMode() == EmberEntity::PositioningMode::PROJECTILE) {
		mScene.deregisterEntityWithTechnique(mEntity, "projectile");
	}
}

void ModelRepresentation::setVisualize(const std::string& visualization, bool visualize) {
}

bool ModelRepresentation::getVisualize(const std::string& visualization) const {
	return false;
}

void ModelRepresentation::reactivatePartActions() {
	ModelPartReactivatorVisitor visitor;
	mMapping.getBaseCase().accept(visitor);
}

void ModelRepresentation::notifyTransformsChanged() {
	auto nodeProvider = mModel->getNodeProvider();
	if (nodeProvider) {
		mBulletCollisionDetector->updateTransforms(Convert::toWF<WFMath::Point<3>>(nodeProvider->getNode()->_getDerivedPosition()),
												   Convert::toWF(nodeProvider->getNode()->_getDerivedOrientation()));
		mBulletCollisionDetector->updateScale(Convert::toWF<WFMath::Vector<3>>(nodeProvider->getScale()));
	}

}

void ModelRepresentation::updateCollisionDetection() {
	mBulletCollisionDetector->clear();
	if (mModel->getNodeProvider()) {
		for (auto& subModel: mModel->getSubmodels()) {
			auto meshPtr = subModel->getEntity()->getMesh();
			auto collisionShape = mScene.getBulletWorld().createMeshShape(meshPtr);
			if (collisionShape) {
				mBulletCollisionDetector->addCollisionShape(std::move(collisionShape));
			}
		}
	}
	notifyTransformsChanged();

}

BulletCollisionDetector& ModelRepresentation::getCollisionDetector() {
	return *mBulletCollisionDetector;
}

Model* ModelRepresentation::getModelForEntity(EmberEntity& entity) {
	IGraphicalRepresentation* representation = entity.getGraphicalRepresentation();
	if (representation && representation->getType() == ModelRepresentation::getTypeNameForClass()) {
		return &dynamic_cast<ModelRepresentation*>(representation)->getModel();
	}
	return nullptr;
}

ModelRepresentation* ModelRepresentation::getRepresentationForEntity(EmberEntity& entity) {
	IGraphicalRepresentation* representation = entity.getGraphicalRepresentation();
	if (representation && representation->getType() == ModelRepresentation::getTypeNameForClass()) {
		return dynamic_cast<ModelRepresentation*>(representation);
	}
	return nullptr;
}

void ModelRepresentation::playSoundForMovementAction(Action* action) {

	if (mCurrentMovementSound) {
		mCurrentMovementSound->stop();
		mCurrentMovementSound = nullptr;
	}
	if (!action->soundAction.definition.sounds.empty()) {
		SoundService::SoundGroup soundGroup{.sounds={},
				.repeating=action->soundAction.definition.repeating,
				.gain=action->soundAction.definition.gain,
				.rolloff=action->soundAction.definition.rolloff,
				.reference=action->soundAction.definition.reference
		};

		if (action->soundAction.definition.order == SoundOrder::RANDOM) {

			//Get one at random.
			auto random_value = std::rand() / ((RAND_MAX + 1u) / action->soundAction.definition.sounds.size());
			auto resWrapper = SoundService::getSingleton().getResourceProvider()->getResource(action->soundAction.definition.sounds[random_value].resourceName);
			if (resWrapper.hasData()) {
				std::shared_ptr<StaticSoundSample> sample = StaticSoundSample::create(resWrapper);
				if (sample) {
					soundGroup.sounds.emplace_back(SoundService::Sound{.soundSample= sample});
				}
			}
		} else {
			for (const auto& soundDef: action->soundAction.definition.sounds) {
				auto resWrapper = SoundService::getSingleton().getResourceProvider()->getResource(soundDef.resourceName);
				if (resWrapper.hasData()) {
					std::shared_ptr<StaticSoundSample> sample = StaticSoundSample::create(resWrapper);
					if (sample) {
						soundGroup.sounds.emplace_back(SoundService::Sound{.soundSample= sample});
					}
				}
			}
		}
		if (!soundGroup.sounds.empty()) {
			mCurrentMovementSound = SoundService::getSingleton().playSound(soundGroup);
			if (mEntity.getPredictedPos().isValid()) {
				mCurrentMovementSound->setPosition(mEntity.getViewPosition());
			}
			if (mEntity.getVelocity().isValid()) {
				mCurrentMovementSound->setVelocity(mEntity.getVelocity());
			}

		}
	}
}

void ModelRepresentation::playSoundForAction(const SoundAction& soundAction) {
	if (!soundAction.definition.sounds.empty()) {
		SoundService::SoundGroup soundGroup{
				.sounds={},
				.repeating=false, //Action sounds are never repeating
				.gain = soundAction.definition.gain,
				.rolloff = soundAction.definition.rolloff,
				.reference = soundAction.definition.reference
		};

		if (soundAction.definition.order == SoundOrder::RANDOM) {

			//Get one at random.
			auto random_value = std::rand() / ((RAND_MAX + 1u) / soundAction.definition.sounds.size());
			auto resWrapper = SoundService::getSingleton().getResourceProvider()->getResource(soundAction.definition.sounds[random_value].resourceName);
			if (resWrapper.hasData()) {
				std::shared_ptr<StaticSoundSample> sample = StaticSoundSample::create(resWrapper);
				if (sample) {
					soundGroup.sounds.emplace_back(SoundService::Sound{.soundSample= sample});
				}
			}
		} else {
			for (const auto& soundDef: soundAction.definition.sounds) {
				auto resWrapper = SoundService::getSingleton().getResourceProvider()->getResource(soundDef.resourceName);
				if (resWrapper.hasData()) {
					std::shared_ptr<StaticSoundSample> sample = StaticSoundSample::create(resWrapper);
					if (sample) {
						soundGroup.sounds.emplace_back(SoundService::Sound{.soundSample= sample});
					}
				}
			}
		}
		if (!soundGroup.sounds.empty()) {
			mCurrentMovementSound = SoundService::getSingleton().playSound(soundGroup);
			//We set the position and velocity here, but won't be updating them as this sound is "one off".
			//It's also detached, so we don't keep a reference to it and rely on the SoundService to remove it once it's done.
			if (mEntity.getPredictedPos().isValid()) {
				mCurrentMovementSound->setPosition(mEntity.getViewPosition());
			}
			if (mEntity.getVelocity().isValid()) {
				mCurrentMovementSound->setVelocity(mEntity.getVelocity());
			}

		}
	}
}

}


