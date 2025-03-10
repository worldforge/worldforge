// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#include "Monitors.h"

#include "Variable.h"

#include <iostream>
#include <mutex>
#include "SynchedState_impl.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

Monitors::Monitors() = default;

Monitors::~Monitors() = default;


void Monitors::insert(std::string key, const Element& val) {
	mState.withState([&](auto state) { state->pairs[std::move(key)] = val; });
}

void Monitors::watch(std::string name, std::unique_ptr<VariableBase> monitor) {
	mState.withState([&](auto state) { state->variableMonitors[std::move(name)] = std::move(monitor); });
}

void Monitors::watch(std::string name, int& value) {
	watch(std::move(name), std::make_unique<Variable<int>>(value));
}

void Monitors::watch(std::string name, long& value) {
	watch(std::move(name), std::make_unique<Variable<long>>(value));
}

void Monitors::watch(std::string name, float& value) {
	watch(std::move(name), std::make_unique<Variable<float>>(value));
}

static std::ostream& operator<<(std::ostream& s, const Element& e) {
	switch (e.getType()) {
		case Atlas::Message::Element::TYPE_INT:
			s << e.Int();
			break;
		case Atlas::Message::Element::TYPE_FLOAT:
			s << e.Float();
			break;
		case Atlas::Message::Element::TYPE_STRING:
			s << e.String();
			break;
		default:
			break;
	}
	return s;
}

void Monitors::send(std::ostream& io) const {
	mState.withStateConst<void>([&](auto state) {
		for (auto& entry: state->pairs) {
			io << entry.first << " " << entry.second << "\n";
		}

		for (auto& entry: state->variableMonitors) {
			io << entry.first << " ";
			entry.second->send(io);
			io << "\n";
		}
	});
}

void Monitors::sendNumerics(std::ostream& io) const {

	mState.withStateConst([&](auto state) {
		for (auto& entry: state->pairs) {
			if (entry.second.isNum()) {
				io << entry.first << " " << entry.second << "\n";
			}
		}

		for (auto& entry: state->variableMonitors) {
			if (entry.second->isNumeric()) {
				io << entry.first << " ";
				entry.second->send(io);
				io << "\n";
			}
		}
	});
}

int Monitors::readVariable(const std::string& key, std::ostream& out_stream) const {
	return mState.withStateConst<int>([&](auto state) {
		auto J = state->variableMonitors.find(key);
		if (J != state->variableMonitors.end()) {
			J->second->send(out_stream);
			return 0;
		}

		auto I = state->pairs.find(key);
		if (I != state->pairs.end()) {
			out_stream << I->second;
			return 0;
		}

		return 1;
	});
}
