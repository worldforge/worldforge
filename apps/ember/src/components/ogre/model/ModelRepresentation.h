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

#ifndef MODELREPRESENTATION_H_
#define MODELREPRESENTATION_H_

#include "ModelDefinition.h"
#include "components/ogre/OgreIncludes.h"
#include "components/ogre/IAnimated.h"
#include "domain/IGraphicalRepresentation.h"
#include "domain/EmberEntity.h"
#include "services/sound/SoundService.h"
#include "ModelAction.h"
#include <OgreVector.h>
#include <sigc++/trackable.h>
#include <vector>
#include <components/ogre/EmberEntityUserObject.h>
#include <components/ogre/BulletCollisionDetector.h>


namespace Eris {
class Entity;
}

namespace Ember {
class EmberEntity;
namespace EntityMapping {
class EntityMapping;
}
namespace OgreView {
class SoundEntity;

class Scene;

namespace Model {

struct ActionDefinition;
struct SoundDefinition;

class Model;

struct Action;

class ModelMount;


/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 *
 * @brief Represents an entity through a Model instance.
 *
 * An instance of this binds an EmberEntity instance and a Model instance together. It's normally not meant to be created externally.
 * You can access instances of it attached to an entity through the ModelRepresentationManager class.
 */
class ModelRepresentation : public IGraphicalRepresentation, public virtual sigc::trackable, public IAnimated {
public:

	/**
	 * @brief The name of the normal standing action.
	 */
	static const char* const ACTION_STAND;

	/**
	 * @brief The name of the forward running action.
	 */
	static const char* const ACTION_RUN;

	/**
	 * @brief The name of the right side stepping running action.
	 */
	static const char* const ACTION_RUN_RIGHT;

	/**
	 * @brief The name of the left side stepping running action.
	 */
	static const char* const ACTION_RUN_LEFT;

	/**
	 * @brief The name of the backwards running action.
	 */
	static const char* const ACTION_RUN_BACKWARDS;

	/**
	 * @brief The name of the walking action.
	 */
	static const char* const ACTION_WALK;

	/**
	 * @brief The name of the right side stepping action.
	 */
	static const char* const ACTION_WALK_RIGHT;

	/**
	 * @brief The name of the left side stepping action.
	 */
	static const char* const ACTION_WALK_LEFT;

	/**
	 * @brief The name of the backwards walking action.
	 */
	static const char* const ACTION_WALK_BACKWARDS;

	/**
	 * @brief The name of the swimming action.
	 */
	static const char* const ACTION_SWIM;

	/**
	 * @brief The name of the water treading action.
	 */
	static const char* const ACTION_TREAD_WATER;

	/**
	 * @brief The name of the float action.
	 */
	static const char* const ACTION_FLOAT;

	/**
	 * @brief Ctor.
	 * @param entity The entity instance to bind to.
	 * @param model The model instance used.
	 * @param scene The scene to which the entity belongs.
	 * @param mapping The model mapping tied to the model.
	 */
	ModelRepresentation(EmberEntity& entity, std::unique_ptr<Model> model, Scene& scene, EntityMapping::EntityMapping& mapping);

	/**
	 * @brief Dtor.
	 */
	~ModelRepresentation() override;

	/**
	 * @brief Initialize position and scaling of the scale node with values from the Model, as well as set up any alternative rendering techniques.
	 */
	void initFromModel();

	/**
	 * @copydoc OgreView::IGraphicalRepresentation::getType()
	 */
	const std::string& getType() const override;

	/**
	 * @brief Gets the shared class type name, which is the same one returned through getType().
	 * This can be used to do lookups to see whether any instance of IGraphicalRepresentation if an instance of this class.
	 * @returns The string "ModelRepresentation".
	 */
	static const std::string& getTypeNameForClass();

	/**
	 * @brief Gets the entity which this representation is connected to.
	 */
	EmberEntity& getEntity() const;

	/**
	 * @brief Gets the model which this representation shows.
	 */
	Model& getModel() const;

	//	void attachToPointOnModel(const std::string& point, Model* model, Ogre::Quaternion orientation = Ogre::Quaternion::IDENTITY, Ogre::Vector3 offset = Ogre::Vector3::ZERO);
	//	void detachFromModel();

	/**
	 * @brief Updates the animation. This is normally called by MotionManager.
	 * @param timeSlice time to update with.
	 */
	void updateAnimation(float timeSlice) override;

	/**
	 * @brief General method for turning on and off debug visualizations. Subclasses might support more types of visualizations than the ones defined here.
	 * @param visualization The type of visualization. Currently supports "OgreBBox".
	 * @param visualize Whether to visualize or not.
	 */
	void setVisualize(const std::string& visualization, bool visualize) override;

	/**
	 * @brief Gets whether a certain visualization is turned on or off.
	 * @param visualization The type of visualization. Currently supports "OgreBBox".
	 * @return true if visualization is turned on, else false
	 */
	bool getVisualize(const std::string& visualization) const override;

	/**
	 * @brief Shows/hides a certain part of the model.
	 * This method is normally called by an instance of EmberEntityPartAction.
	 * @param partName The part to show.
	 * @param visible Whether to have it visible or not.
	 */
	void setModelPartShown(const std::string& partName, bool visible);

	Ogre::Vector3 getScale() const;

	/**
	 * @brief Sets the velocity, in local units. This means relative to the way the representation is facing, so that x>1 means a forward looking direction.
	 */
	void setLocalVelocity(const WFMath::Vector<3>& velocity);

	void notifyTransformsChanged();

	BulletCollisionDetector& getCollisionDetector();

	Scene& getScene() { return mScene; }

	const Scene& getScene() const { return mScene; }


	static Model* getModelForEntity(EmberEntity& entity);

	static ModelRepresentation* getRepresentationForEntity(EmberEntity& entity);

protected:

	/**
	 * @brief The entity which this representation is bound to.
	 */
	EmberEntity& mEntity;

	/**
	 * @brief The model of the entity.
	 * This is the main graphical representation of this entity.
	 * Note that the Model won't be directly connected to the main scene node, instead the mModelMount instance will take care of setting everything up to use an intermediary "scale node".
	 */
	std::unique_ptr<Model> mModel;

	/**
	 * @brief The scene to which the entity belongs.
	 */
	Scene& mScene;

	/**
	 * @brief An entity mapping, which will be reactivated when the model is reloaded.
	 */
	EntityMapping::EntityMapping& mMapping;

	/**
	 The current movement action of the entity, for example a walk action or a run action.
	 */
	Action* mCurrentMovementAction;

	std::shared_ptr<SoundService::SoundControl> mCurrentMovementSound;

	/**
	 All the active actions, except the movement action (since it's stored in mCurrentMovementAction).
	 These actions will be updated each frame.
	 NOTE: we currently don't allow for multiple actions playing at the same time
	 */
	Action* mActiveAction;

	std::shared_ptr<EmberEntityUserObject> mUserObject;

	std::unique_ptr<BulletCollisionDetector> mBulletCollisionDetector;


	/**
	 * A list of operation to process once the Model has completed loading.
	 */
	std::vector<Atlas::Objects::Operation::RootOperation> mPendingEntityOperations;

	/**
	 * @brief The type name for the class.
	 */
	static std::string sTypeName;

	/**
	 *   creates EmberEntityUserObjects, connects them and sets up the collision detection system
	 * @return
	 */
	void connectEntities();

	void entity_Changed(const std::set<std::string>& attributeIds);


	void entity_PositioningModeChanged(EmberEntity::PositioningMode newMode);


	void attrChanged(const std::string& str, const Atlas::Message::Element& v);

	void entity_Operation(const Atlas::Objects::Operation::RootOperation& op);

	void entity_ActionsChanged(const std::vector<ActionChange>& actionChanges);

	Action* getActionForOperation(const Atlas::Objects::Operation::RootOperation& op) const;

	/**
	 * @brief When the Model is reloaded we need to update with the new values.
	 */
	void model_Reloaded();

	/**
	 * @brief When the Model is reset we need to clean up and remove all attachments from it.
	 */
	void model_Resetting();

	/**
	 * @brief Parses and sets the movement mode.
	 * The movement mode is determined mainly from whether the entity is moving or not. The speed of the movement also affects the mode.
	 * @param velocity The velocity of the entity.
	 */
	void parseMovementMode(const WFMath::Vector<3>& velocity);

	/**
	 * @brief Reactivate all previously activated parts for the model.
	 * This is needed whenever the model is changed, because the new model used will then start out with only the default parts activated.
	 */
	void reactivatePartActions();

	/**
	 * @brief Resets all current movement, action and task animations.
	 */
	void resetAnimations();

	void playSoundForMovementAction(Action* action);

	/**
	 * Gets the suitable action for the supplied movement.
	 *
	 * This system will employ some fallbacks, so that if no perfect action can be found an alternative is chosen.
	 * For example, if there's no action for walking sideways an action for moving forward will be used instead.
	 * @param velocity The velocity of the entity.
	 * @return An action, or null if no suitable could be found.
	 */
	Action* getActionForMovement(const WFMath::Vector<3>& velocity) const;

	/**
	 * Searches for actions using their activations, and returns the first one found.
	 * @param type The type of activation for the action.
	 * @param actions A list of activation names, searched for in order.
	 * @return An action, or null if none could be found.
	 */
	Action* getFirstAvailableAction(ActivationDefinition::Type type, std::initializer_list<const char* const> actions) const;

	void updateCollisionDetection();

	void playSoundForAction(const SoundAction& soundAction);


	Action* processEntityOperation(const Atlas::Objects::Operation::RootOperation& op);

	Action* processActivation(ActivationDefinition::Type type, const std::string& activityName);

	void processAddedAction(const std::string& actionName);
};

}
}
}
#endif /* MODELREPRESENTATION_H_ */
