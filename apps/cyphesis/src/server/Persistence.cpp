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


#include "Persistence.h"

#include "Admin.h"
#include "Player.h"
#include "ServerAccount.h"

#include "rules/simulation/LocatedEntity.h"

#include "common/id.h"
#include "common/globals.h"
#include "common/Database.h"


using Atlas::Message::MapType;
using Atlas::Objects::Root;

static constexpr auto debug_flag = false;

Persistence::Persistence(Database& database)
		: m_db(database) {
}

bool Persistence::findAccount(const std::string& name) {
	std::string namestr = "'" + name + "'";
	DatabaseResult dr = m_db.selectSimpleRowBy("accounts", "username", namestr);
	if (dr.error()) {
		spdlog::error("Failure while finding account '{}'.", name);
		return false;
	}
	if (dr.empty()) {
		return false;
	}
	if (dr.size() > 1) {
		spdlog::error("Duplicate username in accounts database for name '{}'.", name);
	}
	return true;
}

std::unique_ptr<Account> Persistence::getAccount(const std::string& name) {
	std::string namestr = "'" + name + "'";
	DatabaseResult dr = m_db.selectSimpleRowBy("accounts", "username", namestr);
	if (dr.error()) {
		spdlog::error("Failure while finding account '{}'.", name);
		return nullptr;
	}
	if (dr.empty()) {
		return nullptr;
	}
	if (dr.size() > 1) {
		spdlog::error("Duplicate username in accounts database for name '{}'.", name);
	}
	auto first = dr.begin();
	const char* c = first.column("id");
	if (c == nullptr) {
		spdlog::error("Unable to find id field in accounts database.");
		return nullptr;
	}
	RouterId id(c);
	c = first.column("password");
	if (c == nullptr) {
		spdlog::error("Unable to find password field in accounts database.");
		return nullptr;
	}
	std::string passwd = c;
	c = first.column("type");
	if (c == nullptr) {
		spdlog::error("Unable to find type field in accounts database.");
		return nullptr;
	}
	std::string type = c;
	if (type == "admin") {
		return std::make_unique<Admin>(nullptr, name, passwd, id);
	} else if (type == "server") {
		return std::make_unique<ServerAccount>(nullptr, name, passwd, id);
	} else {
		return std::make_unique<Player>(nullptr, name, passwd, id);
	}
}

void Persistence::putAccount(const Account& ac) {
	std::string columns = "username, type, password";
	std::string values = "'";
	values += ac.username();
	values += "', '";
	values += ac.getType();
	values += "', '";
	values += ac.password();
	values += "'";
	m_db.createSimpleRow("accounts", ac.getIdAsString(), columns, values);
}



