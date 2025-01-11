// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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


#include "PropertyRuleHandler.h"
#include "ServerPropertyManager.h"

#include "EntityBuilder.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/PropertyManager_impl.h"
#include "common/PropertyFactory_impl.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;


static constexpr auto debug_flag = false;

PropertyRuleHandler::PropertyRuleHandler(PropertyManager<LocatedEntity>& propertyManager)
        : m_propertyManager(propertyManager)
{
}


int PropertyRuleHandler::check(const Atlas::Objects::Root& desc)
{
    assert(!desc->getParent().empty());
    if (desc->getObjtype() != "type") {
        return -1;
    }
    return 0;
}

int PropertyRuleHandler::install(const std::string& name,
                                 const std::string& parent,
                                 const Atlas::Objects::Root& desc,
                                 std::string& dependent,
                                 std::string& reason,
                                 std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes)
{
    assert(desc->getObjtype() == "type");
    if (m_propertyManager.getPropertyFactory(name) != nullptr) {
        spdlog::error("Property rule \"{}\" already exists.", name);
        return -1;
    }
    auto parent_factory = m_propertyManager.getPropertyFactory(parent);
    if (parent_factory == nullptr) {
        dependent = parent;
        reason = fmt::format("Property rule \"{}\" has parent \"{}\" which does "
                         "not exist.", name, parent);
        return 1;
    }
    auto factory = parent_factory->duplicateFactory();
    assert(factory.get() != nullptr);
    m_propertyManager.installFactory(name, desc, std::move(factory));
    return 0;
}

int PropertyRuleHandler::update(const std::string& name,
                                const Atlas::Objects::Root& desc,
                                std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes)
{
    // There is not anything to be modified yet.
    return 0;
}
