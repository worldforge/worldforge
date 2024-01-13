#ifndef ERIS_VIEW_H
#define ERIS_VIEW_H

// WF
#include "Factory.h"
#include "ViewEntity.h"
#include "Router.h"
#include <Atlas/Objects/ObjectsFwd.h>
#include <wfmath/timestamp.h>

// sigc++
#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/slot.h>
#include <sigc++/connection.h>

// std
#include <string>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <Atlas/Message/Element.h>
#include <memory>
#include <chrono>

namespace Eris {

class Avatar;

class Entity;

class Connection;

class Task;

class TypeService;

class EventService;

/** View encapsulates the set of entities currently visible to an Avatar,
 as well as those that have recently been seen. It receives visibility-affecting
 ops from the IGRouter, and uses them to update its state and emit signals.
 */
class View : public Router, public sigc::trackable {
public:
	explicit View(Avatar& av);

	~View() override;

	RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op) override;

	/**
	Retrieve an entity in the view by id. Returns nullptr if no such entity exists
	in the view.
	*/
	ViewEntity* getEntity(const std::string& eid) const;

	Avatar& getAvatar() const {
		return m_owner;
	}

	/** return the current top-level entity. This will return nullptr
	until the first emission of the TopLevelEntityChanged signal. */
	Entity* getTopLevel() const {
		return m_topLevel;
	}

	/**
	 * @brief Gets the TypeService attached to the view.
	 * @return A typeservice instance.
	 */
	TypeService& getTypeService();

	/**
	 * @brief Gets the TypeService attached to the view.
	 * @return A typeservice instance.
	 */
	TypeService& getTypeService() const;

	/**
	 * @brief Gets the EventService used by the view.
	 * @return The main EventService instance.
	 */
	EventService& getEventService();

	/**
	 * @brief Gets the EventService used by the view.
	 * @return The main EventService instance.
	 */
	EventService& getEventService() const;

	/** once-per-frame update of the View - clients should call this method
	once per game loop (or similar), to allow the View to update Entity
	state. This includes motion prediction for moving entities, and confidence
	levels for disappeared entities.
	*/
	void update(const std::chrono::steady_clock::time_point& currentTime);


	/** get the current local approximation of world time. */
	std::chrono::milliseconds getWorldTime();

	/** We use this to synchronize the local world time up. */
	void updateWorldTime(std::chrono::milliseconds t);

	/**
	Register an Entity Factory with this view
	*/
	void registerFactory(std::unique_ptr<Factory> factory);

	double getSimulationSpeed() const;

	typedef sigc::slot<void(ViewEntity*)> EntitySightSlot;

	/**
	Connect up a slot to be fired when an Entity with the specified ID is seen.
	If the entity is already visible, this is a no-op (and will log an error)
	*/
	sigc::connection notifyWhenEntitySeen(const std::string& eid, const EntitySightSlot& slot);

	/** emitted whenever the View creates a new Entity instance. This signal
	is emitted once the entity has been fully bound into the View */
	sigc::signal<void(ViewEntity*)> EntitySeen;

	/** emitted when a SIGHT(CREATE) op is received for an entity */
	sigc::signal<void(ViewEntity*)> EntityCreated;

	/** emitted when a SIGHT(DELETE) op is received for an entity */
	sigc::signal<void(ViewEntity*)> EntityDeleted;

	/// emitted when the TLVE changes
	sigc::signal<void()> TopLevelEntityChanged;

	/**
	 * Emitted after a new Entity has been created and initialized.
	 *
	 * This signal is mainly meant for debugging or authoring; normal entity
	 * presentation logic should use EntitySeen or EntityCreated instead.
	 */
	sigc::signal<void(ViewEntity*)> InitialSightEntity;

	void dumpLookQueue();

	/**
	Retrieve the current look queue size, for debugging / statistics purposes.
	Eg, this could be displayed as a bar-chart on screen in a client (optionally)
	*/
	std::size_t lookQueueSize() const {
		return m_lookQueue.size();
	}

	/**
	Issue a LOOK operation for the specified entity ID. The id may be
	empty for an anonymous look. The pending sight map will be updated
	with the appropriate information.
	*/
	void sendLookAt(const std::string& eid);

	size_t pruneAbandonedPendingEntities(const std::chrono::steady_clock::time_point& currentTime);

	Connection& getConnection() const;

protected:

	friend class ViewEntity;

	friend class Avatar;

	friend class Task;

	void appear(const std::string& eid, std::chrono::milliseconds stamp);

	void disappear(const std::string& eid);

	void deleteEntity(const std::string& eid);

	void unseen(const std::string& eid);

	/// test if the specified entity ID is pending initial sight on the View
	bool isPending(const std::string& eid) const;

	void addToPrediction(ViewEntity* ent);

	void removeFromPrediction(ViewEntity* ent);

	/**
	Method to register and unregister tasks with with view, so they can
	have their progress updated automatically by update(). Only certain
	tasks (those with linear progress) are handled this way, but all tasks
	are submitted to this method.
	*/
	void taskRateChanged(Task*);

private:
	ViewEntity* initialSight(const Atlas::Objects::Entity::RootEntity& ge);

	void getEntityFromServer(const std::string& eid);

	/** helper to update the top-level entity, fire signals, etc */
	void setTopLevelEntity(Entity* newTopLevel);

	std::unique_ptr<ViewEntity> createEntity(const Atlas::Objects::Entity::RootEntity&);

	void parseSimulationSpeed(const Atlas::Message::Element& element);

	/**
	If the look queue is not empty, pop the first item and send a request
	for it to the server.
	*/
	void issueQueuedLook();

	void eraseFromLookQueue(const std::string& eid);

	/**
	 * Called when a type has been bound, which means that we should replay any queued ops for that type.
	 * @param type
	 */
	void typeBound(TypeInfo* type);

	/**
	 * Called when a type couldn't be bound, which means that we should throw away any queued ops for that type.
	 * This should normally never happen.
	 * @param type
	 */
	void typeBad(TypeInfo* type);

	Avatar& m_owner;

	struct EntityEntry {
		std::unique_ptr<ViewEntity> entity;
	};
	std::unordered_map<std::string, EntityEntry> m_contents;
	Entity* m_topLevel; ///< the top-level visible entity for this view
	std::chrono::steady_clock::time_point m_lastUpdateTime;

	/**
	 * The simulation speed, as determined by the "simulation_speed" property of the top level entity.
	 */
	double m_simulationSpeed;

	struct PendingStatus {

		enum class State {
			IN_LOOK_QUEUE,
			WAITING_FOR_RESPONSE_FROM_SERVER
		};

		State state;
		/**
		 * A list of ops that we received as being FROM this entity. We hold on to these and replay them when the complete
		 * entity info has been sent to us.
		 */
		std::vector<Atlas::Objects::Operation::RootOperation> queuedFromOps;
		std::chrono::steady_clock::time_point registrationTime = std::chrono::steady_clock::now();
	};

	std::map<std::string, PendingStatus> m_pending;

	struct TypeDelayedEntry {
		std::vector<Atlas::Objects::Operation::RootOperation> operations;
		/**
		 * This is used for when we get a bad type and need to clean up any pending entity ids.
		 */
		std::set<std::string> pendingEntityIds;
	};

	std::map<TypeInfo*, TypeDelayedEntry> m_typeDelayedOperations;

	/**
	A queue of entities to be looked at, which have not yet be requested
	from the server. The number of concurrent active LOOK requests is
	capped to avoid network failures.

	@sa m_maxPendingCount
	*/
	std::deque<std::string> m_lookQueue;

	sigc::connection m_simulationSpeedConnection;

	std::chrono::steady_clock::time_point m_stampAtLastOp;
	std::chrono::milliseconds m_lastOpTime;

	unsigned int m_maxPendingCount;

	typedef sigc::signal<void(ViewEntity*)> EntitySightSignal;

	typedef std::unordered_map<std::string, EntitySightSignal> NotifySightMap;
	NotifySightMap m_notifySights;

	typedef std::set<ViewEntity*> EntitySet;

	/** all the entities in the view which are moving, so they can be
	motion predicted. */
	EntitySet m_moving;

	struct FactoryOrdering {
		bool operator()(const std::unique_ptr<Factory>& a, const std::unique_ptr<Factory>& b) const {   // higher priority factories are placed nearer the start
			return a->priority() > b->priority();
		}
	};

	typedef std::multiset<std::unique_ptr<Factory>, FactoryOrdering> FactoryStore;
	FactoryStore m_factories;

	std::set<Task*> m_progressingTasks;
	TypeInfo* m_actionType;

};

} // of namespace Eris

#endif // of ERIS_VIEW_H
