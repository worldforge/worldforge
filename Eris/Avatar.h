#ifndef ERIS_AVATAR_H
#define ERIS_AVATAR_H

#include <Eris/Types.h>
#include <Eris/EntityRef.h>

#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/timestamp.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/connection.h>

namespace Eris
{

// Forward Declerations
class Account;
class IGRouter;
class View;
class Connection;
class TransferInfo;

/** The player's avatar representation. **/
class Avatar : virtual public sigc::trackable
{
public:
    virtual ~Avatar();

    /// Get the Entity id of this Avatar
    const std::string & getId() const;

    /// Get the Entity this Avatar refers to
    EntityPtr getEntity() const;

    View* getView() const;

    Connection* getConnection() const;

    /** get the current local approximation of world time. */
    double getWorldTime();

    const EntityRef& getWielded() const;

    /**
     * @brief Drop an entity in the Avatar's inventory at the given location.
     * @param entity The entity to drop.
     * @param pos The position within the location to place the entity at (if possible by the server).
     * @param orientation The orientation of the new entity. This will only be used if the quaternion sent is valid.
     * @param loc The new location, as an entity id.
     */
    void drop(Entity* entity, const WFMath::Point<3>& pos,
            const WFMath::Quaternion& orientation, const std::string& loc);

    /**
     * @brief Drop an entity in the Avatar's inventory at the Avatar's feet (or actually in the parent entity of the Avatar).
     * @param entity The entity to drop.
     * @param pos The position within the location to place the entity at (if possible by the server).
     * @param orientation The orientation of the new entity. This will only be used if the quaternion sent is valid.
     */
    void drop(Entity* entity, const WFMath::Vector<3>& offset = WFMath::Vector<3>(0, 0, 0),
            const WFMath::Quaternion& orientation = WFMath::Quaternion());

    /// Move an entity into the Avatar's inventory
    void take(Entity*);

    /// Touch an entity
    void touch(Entity*);

    /// Say something (in-game)
    void say(const std::string&);

    /// Emote something (in-game)
    void emote(const std::string&);

    /// Have the character move towards a position
    void moveToPoint(const WFMath::Point<3>&);

    /// Set the character's velocity
    void moveInDirection(const WFMath::Vector<3>&);

    /// Set the character's velocity and orientation
    void moveInDirection(const WFMath::Vector<3>&, const WFMath::Quaternion&);

    /**
     * @brief Place an entity inside another one.
     *
     * @note Use this method both when you want to move an entity from one
     * container to another, or just want to update the position or
     * orientation of an entity without changing its container.
     *
     * @param entity The entity to place.
     * @param container The container for the entity.
     * @param pos The position of the entity within the container.
     * @param orientation An optional orientation of the entity.
     */
    void place(Entity* entity, Entity* container, const WFMath::Point<3>& pos,
            const WFMath::Quaternion& orientation = WFMath::Quaternion());

    /// Wield an entity which is inside the Avatar's inventory
    void wield(Entity * entity);

    /**
     * @brief Use the currently wielded entity (tool) on another entity.
     * @param entity A pointer to the entity you wish to use your tool on.
     * @param position A position where you perform the operation.
     * @param op The operation of the tool to perform, or an empty string to use the default.
     *
     * If @a position is invalid the "pos" parameter will not be set on the USE operation.
     *
     * @sa WFMath::Point< 3 >::Point(), WFMath::Point< 3 >::setValid(), WFMath::Point< 3 >::isValid()
     **/
    void useOn(Entity * entity, const WFMath::Point< 3 > & position, const std::string& op);

    /**
    @brief Attach the specified entity
    @param entity The entity to be attacked
    */
    void attack(Entity* entity);

    /**
    @brief Stop the current task, if one is in progress.
    This could be either a useOn or attack.
    */
    void useStop();

    void deactivate();

    /**
     * @brief Sets whether the current avatar is an admin character.
     *
     * As an "admin" character the avatar has greater ability to alter the state of
     * the server. This is often done by sending Atlas ops to the entity itself, thus
     * bypassing the normal routing rules on the server.
     *
     * It's up to the client to determine which avatars are admin, and set this flag
     * as soon as possible after the Avatar has been created.
     */
    void setIsAdmin(bool isAdmin);

    /**
     * @brief Gets whether the current avatar is an admin character.
     *
     * As an "admin" character the avatar has greater ability to alter the state of
     * the server. This is often done by sending Atlas ops to the entity itself, thus
     * bypassing the normal routing rules on the server.
     *
     * It's up to the client to determine which avatars are admin, and call setIsAdmin
     * as soon as possible after the Avatar has been created.
     */
    bool getIsAdmin();

    /**
    Emitted when the character entity of this Avatar is valid (and presumably,
    visible). This will happen some time after the InGame signal is emitted.
    A client might wish to show some kind of 'busy' animation, eg an hour-glass,
    while waiting for this signal.
    */
    sigc::signal<void, Entity*> GotCharacterEntity;

    // These two signals just transmit the Entity's
    // AddedMember and RemovedMember signals, but
    // you're allowed to connect to them as soon as
    // the Avatar has been created, instead of having to wait
    // for the Entity to be created.

    /// An object was added to the inventory
    sigc::signal<void,Entity*> InvAdded;
    /// An object was removed from the inventory
    sigc::signal<void,Entity*> InvRemoved;

    /** emitted when this Avatar hears something. Passes the source of
    the sound, and the operation that was heard, for example a Talk. */
    sigc::signal<void, Entity*, const Atlas::Objects::Operation::RootOperation&> Hear;
protected:
    friend class Account;

    /** Create a new Avatar object.
    @param pl The player that owns the Avatar
    */
    Avatar(Account& pl, const std::string& entId);

    friend class AccountRouter;
    friend class IGRouter;

    /** called by the IG router for each op it sees with a valid 'seconds'
    attribute set. We use this to synchronize the local world time up. */
    void updateWorldTime(double t);

protected:
    void onEntityAppear(Entity* ent);
    void onCharacterChildAdded(Entity* child);
    void onCharacterChildRemoved(Entity* child);

    void onCharacterWield(const Atlas::Message::Element&);
    void onWieldedChanged();

    void logoutResponse(const Atlas::Objects::Operation::RootOperation&);

    /**
     * @brief Called when a logout of the avatar has been requested by the
     *  server.
     */
    void logoutRequested();

    /**
     * @brief Called when a logout and server transfer of the avatar has been
     *  requested by the server.
     * @param transferInfo The transfer info which contains information about
     *  the server to transfer to.
     */
    void logoutRequested(const TransferInfo& transferInfo);

    Account& m_account;

    std::string m_entityId;
    EntityPtr m_entity;

    WFMath::TimeStamp m_stampAtLastOp;
    double m_lastOpTime;

    IGRouter* m_router;
    View* m_view;

    EntityRef m_wielded;

    sigc::connection m_entityAppearanceCon;

    bool m_isAdmin;
};

inline const std::string & Avatar::getId() const
{
    return m_entityId;
}

inline EntityPtr Avatar::getEntity() const
{
    return m_entity;
}

inline View* Avatar::getView() const
{
    return m_view;
}

inline const EntityRef& Avatar::getWielded() const
{
    return m_wielded;
}
} // of namespace Eris

#endif
