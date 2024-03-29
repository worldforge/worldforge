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


#ifndef CLIENT_BASE_CLIENT_LEGACY_H
#define CLIENT_BASE_CLIENT_LEGACY_H

#include "modules/Ref.h"
#include "ClientConnection.h"

#include <Atlas/Objects/Entity.h>

class CreatorClient;

template<typename>
class PropertyManager;

template<typename>
struct TypeStore;

class MemEntity;

/// \brief Base class for classes that implement clients used to connect to a
/// cyphesis server
/// TODO: remove in favour of common/BaseClient which uses the asio event loop
class BaseClientLegacy {
protected:
	TypeStore<MemEntity>& m_typeStore;
	/// \brief Low level connection to the server
	ClientConnection m_connection;
	/// \brief Client object that manages the creator avatar
	Ref<CreatorClient> m_character;
	/// \brief Store for details of the account after login
	Atlas::Objects::Root m_player;
	/// \brief Name used for the username of the account and the name of avatars
	std::string m_playerName;
	/// \brief Identifier of the Account on the server after login
	std::string m_playerId;

	Atlas::Objects::Entity::RootEntity extractFirstArg(Atlas::Objects::Operation::RootOperation op);

public:
	explicit BaseClientLegacy(boost::asio::io_context& io_context,
							  Atlas::Objects::Factories& factories,
							  TypeStore<MemEntity>& typeStore);

	virtual ~BaseClientLegacy();

	CreatorClient* character() {
		return m_character.get();
	}

	const std::string& id() const {
		return m_playerId;
	}

	Atlas::Objects::Root createSystemAccount();

	Atlas::Objects::Root createAccount(const std::string& name,
									   const std::string& pword);

	Ref<CreatorClient> createCharacter(const std::string& name);

	void logout();

	void handleNet();

	/// \brief Function called when nothing else is going on
	virtual void idle() = 0;

	/// \brief Connect to a local server via a unix socket
	int connectLocal(const std::string& socket) {
		return m_connection.connectLocal(socket);
	}

	/// \brief Connect to a remote server using a network socket
	int connect(const std::string& server, int port) {
		return m_connection.connect(server, static_cast<unsigned short>(port));
	}

	/// \brief Send an operation to the server
	void send(const Atlas::Objects::Operation::RootOperation& op);

	int wait() {
		return m_connection.wait();
	}

	Atlas::Objects::Operation::RootOperation sendAndWaitReply(const Operation& op) {
		OpVector res;
		m_connection.sendAndWaitReply(op, res);
		if (res.empty()) {
			return {};
		} else {
			return res.front();
		}
	}

};

#endif // CLIENT_BASE_CLIENT_LEGACY_H
