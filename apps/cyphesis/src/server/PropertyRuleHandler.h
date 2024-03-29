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


#ifndef SERVER_PROPERTY_RULE_HANDLER_H
#define SERVER_PROPERTY_RULE_HANDLER_H

#include "RuleHandler.h"

class EntityBuilder;
template <typename EntityT>
class PropertyManager;

/// \brief Handle processing and updating of task rules
class PropertyRuleHandler : public RuleHandler
{
    private:
        PropertyManager<LocatedEntity>& m_propertyManager;

    public:

        PropertyRuleHandler(PropertyManager<LocatedEntity>& propertyManager);

        int check(const Atlas::Objects::Root& desc) override;

        int install(const std::string&,
                    const std::string&,
                    const Atlas::Objects::Root& desc,
                    std::string&,
                    std::string&,
                    std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes) override;

        int update(const std::string&,
                   const Atlas::Objects::Root& desc,
                   std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes) override;
};

#endif // SERVER_PROPERTY_RULE_HANDLER_H
