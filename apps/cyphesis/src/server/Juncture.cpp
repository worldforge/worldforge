// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#include "Juncture.h"

#include "CommPeer.h"
#include "Connection.h"
#include "Peer.h"
#include "ServerRouting.h"

#include "common/operations/Connect.h"

#include "common/debug.h"
#include "common/log.h"
#include "common/AtlasFactories.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Login;


static constexpr auto debug_flag = false;


void Juncture::onSocketConnected() {
	auto socket = m_socket.lock();
	m_peer = new Peer(*socket, m_connection->m_server,
					  m_host, m_port, m_id);

	//Transfers ownership to the socket. This feels dangerous, since it's a weak ptr.
	socket->setup(std::unique_ptr<Link>(m_peer));
	m_peer->destroyed.connect(sigc::mem_fun(*this, &Juncture::onPeerLost));
	m_peer->replied.connect(sigc::mem_fun(*this, &Juncture::onPeerReplied));

	spdlog::info("Juncture onPeerC succeeded {}", getIdAsString());
	if (m_connection != nullptr) {
		Anonymous info_arg;
		addToEntity(info_arg);

		Info info;
		info->setArgs1(info_arg);
		if (m_connectRef != 0L) {
			info->setRefno(m_connectRef);
		}

		m_connection->send(info);
	}
	m_socket.reset();
	m_connectRef = 0L;
}

void Juncture::onSocketFailed() {
	assert(m_peer == nullptr);
	assert(!m_socket.expired());
	if (m_connection != nullptr) {
		if (++m_address.iterator != m_address.entries.end()) {
			if (attemptConnect("foo", 6767) == 0) {
				return;
			}
		}

		Anonymous error_arg;
		error_arg->setAttr("message", "Connection failed");

		Error error;
		error->setArgs1(error_arg);
		if (m_connectRef != 0L) {
			error->setRefno(m_connectRef);
		}
		m_connection->send(error);
	}
	m_socket.reset();
	m_connectRef = 0L;
}

void Juncture::onPeerLost() {
}

void Juncture::onPeerReplied(const Operation& op) {
	if (m_connection != nullptr) {
		m_connection->send(op);
	}
}

int Juncture::attemptConnect(const std::string& hostname, int port) {
	auto peer = std::make_shared<CommPeer>(m_connection->m_server.getName(),
										   m_connection->m_commSocket.m_io_context,
										   AtlasFactories::factories);
	m_socket = std::weak_ptr<CommPeer>(peer);

	peer->connect(m_address.iterator->endpoint());

	m_host = hostname;
	m_port = port;

	spdlog::info("Connection in progress {}", getIdAsString());
	peer->connected.connect(sigc::mem_fun(*this,
										  &Juncture::onSocketConnected));
	peer->failed.connect(sigc::mem_fun(*this,
									   &Juncture::onSocketFailed));

	return 0;
}

Juncture::Juncture(Connection* c, RouterId id) :
		ConnectableRouter(std::move(id)),
		m_connection(c),
		m_socket(),
		m_peer(nullptr),
		m_connectRef(0),
		m_port(0) {
}

Juncture::~Juncture() = default;

void Juncture::externalOperation(const Operation& op, Link&) {
	spdlog::error("{} called", __PRETTY_FUNCTION__);
	assert(m_connection != nullptr);
	OpVector reply;
	long serialno = op->getSerialno();
	operation(op, reply);
	if (!reply.empty()) {
		for (auto& replyOp: reply) {
			if (!op->isDefaultSerialno()) {
				// Should we respect existing refnos?
				if (replyOp->isDefaultRefno()) {
					replyOp->setRefno(serialno);
				}
			}
		}
		// FIXME detect socket failure here
		m_connection->send(reply);
	}
}

void Juncture::operation(const Operation& op, OpVector& res) {
	auto op_no = op->getClassNo();
	switch (op_no) {
		case Atlas::Objects::Operation::LOGIN_NO:
			LoginOperation(op, res);
			break;
		default:
			OtherOperation(op, res);
			break;
	}
}

void Juncture::addToEntity(const RootEntity& ent) const {
	ent->setObjtype("obj");
	ent->setId(getIdAsString());
	ent->setParent("juncture");
}

void Juncture::LoginOperation(const Operation& op, OpVector& res) {
	spdlog::info("Juncture got login");

	const std::vector<Root>& args = op->getArgs();
	if (args.empty()) {
		error(op, "No argument to connect op", res, getIdAsString());
		return;
	}
	const Root& arg = args.front();

	Element username_attr;
	if (arg->copyAttr("username", username_attr) != 0 || !username_attr.isString()) {
		error(op, "Argument to connect op has no username", res, getIdAsString());
		return;
	}
	const std::string& username = username_attr.String();

	Element password_attr;
	if (arg->copyAttr("password", password_attr) != 0 || !password_attr.isString()) {
		error(op, "Argument to connect op has no password", res, getIdAsString());
		return;
	}
	const std::string& password = password_attr.String();

	if (m_peer == nullptr) {
		error(op, "Juncture not connected", res, getIdAsString());
		return;
	}
	assert(m_socket.expired());

	if (m_peer->getAuthState() != PEER_INIT) {
		error(op, "Juncture not ready", res, getIdAsString());
		return;
	}

	Anonymous account;
	account->setAttr("username", username);
	account->setAttr("password", password);

	Login l;
	l->setArgs1(account);
	if (!op->isDefaultSerialno()) {
		l->setSerialno(op->getSerialno());
	}
	// Send the login op
	m_peer->send(l);
	m_peer->setAuthState(PEER_AUTHENTICATING);
}

void Juncture::OtherOperation(const Operation& op, OpVector& res) {
	const int op_type = op->getClassNo();
	if (op_type == Atlas::Objects::Operation::CONNECT_NO) {
		return customConnectOperation(op, res);
	}
}

void Juncture::customConnectOperation(const Operation& op, OpVector& res) {
	spdlog::info("Juncture got connect");

	if (m_peer != nullptr) {
		error(op, "Juncture already connected", res, getIdAsString());
		return;
	}
	assert(m_socket.expired());

	const std::vector<Root>& args = op->getArgs();
	if (args.empty()) {
		error(op, "No argument to connect op", res, getIdAsString());
		return;
	}
	const Root& arg = args.front();
	Element hostname_attr;
	if (arg->copyAttr("hostname", hostname_attr) != 0 ||
		!hostname_attr.isString()) {
		error(op, "Argument to connect op has no hostname", res, getIdAsString());
		return;
	}
	const std::string& hostname = hostname_attr.String();

	Element port_attr;
	if (arg->copyAttr("port", port_attr) != 0 || !port_attr.isInt()) {
		error(op, "Argument to connect op has no port", res, getIdAsString());
		return;
	}
	auto port = port_attr.Int();

	cy_debug_print("Connecting to " << hostname)
	m_address = PeerAddress{};

	boost::asio::ip::tcp::resolver resolver(m_connection->m_commSocket.m_io_context);

	try {
		m_address.entries = resolver.resolve(hostname, fmt::format("{}", port));
		m_address.iterator = m_address.entries.begin();
	} catch (const std::exception& e) {
		error(op, "Could not connect to peer host.", res, getIdAsString());
		return;
	}

	m_connectRef = op->getSerialno();

	if (attemptConnect(hostname, static_cast<int>(port)) != 0) {
		error(op, "Connection failed", res, getIdAsString());
	}
}

int Juncture::teleportEntity(const LocatedEntity* ent) {
	if (m_peer == nullptr) {
		spdlog::error("Attempt to teleport through disconnected juncture");
		return -1;
	}
	return m_peer->teleportEntity(ent);
}

void Juncture::setConnection(Connection* connection) {
	m_connection = connection;
}

Connection* Juncture::getConnection() const {
	return m_connection;
}
