/*
 Copyright (C) 2025 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#pragma once

#include "common/Property.h"
#include "Entity.h"

class WorldProperty : public Property<Atlas::Message::MapType, LocatedEntity>
{
public:
    static constexpr auto property_name = "world";

    WorldProperty();

    void install(LocatedEntity&, const std::string&) override;
    void remove(LocatedEntity&, const std::string& name) override;

    HandlerResult operation(LocatedEntity&,
                            const Operation&,
                            OpVector&) override;

private:
    void LookOperation(const LocatedEntity& entity, const Operation& op, OpVector& res) const;
    void DeleteOperation(LocatedEntity& entity, const Operation& op, OpVector& res);
    void RelayOperation(const LocatedEntity& entity, const Operation& op, OpVector& res);

    /// \brief Keeps track of relayed operations.
    struct Relay
    {
        /// \brief The entity to which the operation was relayed.
        std::string entityId;
        /// \brief A callback to call when a response is received.
        sigc::slot<void(const Operation&, const std::string&)> callback;
    };

    /// \brief Keeps track of serial numbers for relayed ops.
    long int m_serialNumber;

    /**
     * \brief A store of registered outgoing relays for the world.
     *
     * Key is the serialno/refno of the Relay op.
     */
    std::map<long int, Relay> m_relays;

    /**
     * @brief Clears the world of all entities and properties.
     *
     * Once this method has been run the server should be reset back to
     * a "clean" state.
     */
    void clearWorld(LocatedEntity& entity, OpVector& res);
};
