// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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


#include "Property_impl.h"
#include "Property.h"
#include "log.h"
#include "ModifierType.h"

template
struct PropertyAtlasBase<int>;

template
struct PropertyAtlasBase<long>;

template
struct PropertyAtlasBase<float>;

template
struct PropertyAtlasBase<double>;

template
struct PropertyAtlasBase<std::string>;

template
struct PropertyAtlasBase<Atlas::Message::ListType>;

template
struct PropertyAtlasBase<Atlas::Message::MapType>;

template <> const std::string PropertyAtlasBase<int>::property_atlastype = "int";
template <> const std::string PropertyAtlasBase<long>::property_atlastype = "int";
template <> const std::string PropertyAtlasBase<float>::property_atlastype = "float";
template <> const std::string PropertyAtlasBase<double>::property_atlastype = "float";
template <> const std::string PropertyAtlasBase<std::string>::property_atlastype = "string";
template <> const std::string PropertyAtlasBase<Atlas::Message::ListType>::property_atlastype = "list";
template <> const std::string PropertyAtlasBase<Atlas::Message::MapType>::property_atlastype = "map";
