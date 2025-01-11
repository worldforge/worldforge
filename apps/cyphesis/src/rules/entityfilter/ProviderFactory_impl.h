/*
 Copyright (C) 2020 Erik Ogenvik

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

#include "ProviderFactory.h"
#include "Providers_impl.h"
#include "Filter_impl.h"
#include "common/log.h"

namespace EntityFilter {

template<typename EntityT>
std::shared_ptr<Consumer<QueryContext<EntityT>>> ProviderFactory<EntityT>::createProvider(Segment segment) const {
	return createProviders(SegmentsList{std::move(segment)});
}

template<typename EntityT>
std::shared_ptr<Consumer<QueryContext<EntityT>>> ProviderFactory<EntityT>::createProviders(SegmentsList segments) const {
	if (!segments.empty()) {
		auto& first_attribute = segments.front().attribute;
		if (first_attribute == "entity") {
			return createEntityProvider<EntityProvider<EntityT>>(std::move(segments));
		} else if (first_attribute == "self") {
			return createSelfEntityProvider(std::move(segments));
		} else if (first_attribute == "types") {
			return createDynamicTypeNodeProvider(std::move(segments));
		} else if (first_attribute == "actor") {
			return createEntityProvider<ActorProvider<EntityT>>(std::move(segments));
		} else if (first_attribute == "tool") {
			return createEntityProvider<ToolProvider<EntityT>>(std::move(segments));
		} else if (first_attribute == "child") {
			return createEntityProvider<ChildProvider<EntityT>>(std::move(segments));
		} else if (first_attribute == "memory") {
			return createMemoryProvider(std::move(segments));
		} else if (first_attribute == "entity_location") {
			return createEntityProvider<EntityLocationProvider<EntityT>>(std::move(segments));
		}
	}
	return nullptr;
}


template<typename EntityT>
std::shared_ptr<MemoryProvider<EntityT>> ProviderFactory<EntityT>::createMemoryProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}
	segments.pop_front();
	return std::make_shared<MemoryProvider<EntityT>>(createMapProvider(std::move(segments)));
}

template<typename EntityT>
std::shared_ptr<Consumer<QueryContext<EntityT>>>
ProviderFactory<EntityT>::createGetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext<EntityT>>> entity_provider, SegmentsList segments) const {
	if (segments.empty()) {
		return std::make_shared<GetEntityFunctionProvider<EntityT>>(std::move(entity_provider), nullptr);
	}
	return std::make_shared<GetEntityFunctionProvider<EntityT>>(std::move(entity_provider), createPropertyProvider(std::move(segments)));
}

template<typename EntityT>
std::shared_ptr<Consumer<QueryContext<EntityT>>> ProviderFactory<EntityT>::createSimpleGetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext<EntityT>>> entity_provider) const {
	return std::make_shared<GetEntityFunctionProvider<EntityT>>(std::move(entity_provider), nullptr);
}

template<typename EntityT>
std::shared_ptr<DynamicTypeNodeProvider<EntityT>> ProviderFactory<EntityT>::createDynamicTypeNodeProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}
	segments.pop_front();
	//A little hack here to avoid calling yet another method.
	if (segments.empty()) {
		return nullptr;
	}
	auto type = segments.front().attribute;
	segments.pop_front();
	return std::make_shared<DynamicTypeNodeProvider<EntityT>>(createTypeNodeProvider(std::move(segments)), type);
}

template<typename EntityT>
template<typename T>
std::shared_ptr<T> ProviderFactory<EntityT>::createEntityProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}
	segments.pop_front();
	if (segments.empty()) {
		return std::make_shared<T>(nullptr);
	}
	return std::make_shared<T>(createPropertyProvider(std::move(segments)));
}

template<typename EntityT>
std::shared_ptr<SelfEntityProvider<EntityT>> ProviderFactory<EntityT>::createSelfEntityProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}
	segments.pop_front();
	return std::make_shared<SelfEntityProvider<EntityT>>(createPropertyProvider(std::move(segments)));
}

template<typename EntityT>
std::shared_ptr<Consumer<EntityT>> ProviderFactory<EntityT>::createPropertyProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}

	auto segment = std::move(segments.front());
	auto attr = segment.attribute;

	segments.pop_front();

	if (segment.delimiter == ":") {
		return std::make_shared<SoftPropertyProvider<EntityT>>(createMapProvider(std::move(segments)), attr);
	} else {

		if (attr == "type") {
			return std::make_shared<EntityTypeProvider<EntityT>>(createTypeNodeProvider(std::move(segments)));
		} else if (attr == "id") {
			return std::make_shared<EntityIdProvider<EntityT>>();
		} else if (attr == "bbox") {
			return createBBoxProvider(std::move(segments));
		} else if (attr == "contains") {
			return std::make_shared<ContainsProvider<EntityT>>();
		} else {
			return std::make_shared<SoftPropertyProvider<EntityT>>(createMapProvider(std::move(segments)), attr);
		}
	}
}

template<typename EntityT>
std::shared_ptr<BBoxProvider<EntityT>> ProviderFactory<EntityT>::createBBoxProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}

	auto& segment = segments.front();
	auto attr = segment.attribute;

	auto measurement_extractor = [&]() -> typename BBoxProvider<EntityT>::Measurement {
		if (attr == "width") {
			return BBoxProvider<EntityT>::Measurement::WIDTH;
		} else if (attr == "depth") {
			return BBoxProvider<EntityT>::Measurement::DEPTH;
		} else if (attr == "height") {
			return BBoxProvider<EntityT>::Measurement::HEIGHT;
		} else if (attr == "volume") {
			return BBoxProvider<EntityT>::Measurement::VOLUME;
		} else if (attr == "area") {
			return BBoxProvider<EntityT>::Measurement::AREA;
		}
		throw std::invalid_argument(fmt::format("Could not compile query as '{}' isn't a valid measurement for a Bounding Box.", attr));
	};

	segments.pop_front();

	return std::make_shared<BBoxProvider<EntityT>>(createMapProvider(std::move(segments)), measurement_extractor());

}

template<typename EntityT>
std::shared_ptr<MapProvider> ProviderFactory<EntityT>::createMapProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}

	auto segment = std::move(segments.front());
	auto attr = segment.attribute;

	segments.pop_front();

	return std::make_shared<MapProvider>(createMapProvider(std::move(segments)), attr);
}

template<typename EntityT>
std::shared_ptr<TypeNodeProvider<EntityT>> ProviderFactory<EntityT>::createTypeNodeProvider(SegmentsList segments) const {
	if (segments.empty()) {
		return nullptr;
	}

	auto& segment = segments.front();
	auto attr = segment.attribute;

	return std::make_shared<TypeNodeProvider<EntityT>>(attr);
}

}