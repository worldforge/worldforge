// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include "common/Router.h"
#include "ConnectableRouter.h"

class Account;

class ServerRouting;

/// \brief This class handles the default out-of-game chat area that all
/// Account objects that are currently logged in are subscribed to by default.
///
/// This allows chat between all connected players, and provides a forum for
/// discusion out of the context of the game.
class Lobby : public Router, public AtlasDescribable {
private:
	std::map<std::string, ConnectableRouter*> m_accounts;
	ServerRouting& m_server;
public:
	explicit Lobby(ServerRouting& serverRouting, RouterId id);

	~Lobby() override;

	void addAccount(ConnectableRouter* a);

	void removeAccount(ConnectableRouter* a);

	/// Accessor for Accounts map.
	const std::map<std::string, ConnectableRouter*>& getAccounts() const {
		return m_accounts;
	}

	void operation(const Operation&, OpVector&) override;

	void addToEntity(const Atlas::Objects::Entity::RootEntity&) const override;
};

#endif // SERVER_LOBBY_H
