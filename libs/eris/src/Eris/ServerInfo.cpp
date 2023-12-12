
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ServerInfo.h"

#include "Log.h"

#include <Atlas/Objects/Entity.h>
#include <ranges>

using Atlas::Objects::Entity::RootEntity;

namespace Eris {

void ServerInfo::processServer(const RootEntity& svr) {
	Atlas::Message::Element element;

	if (!svr->copyAttr("ruleset", element) && element.isString()) {
		ruleset = element.String();
	} else {
		return;
	}

	name = svr->getName();
	if (!svr->copyAttr("clients", element) && element.isInt()) {
		clients = (int) element.Int();
	} else {
		return;
	}
	if (!svr->copyAttr("server", element) && element.isString()) {
		server = element.String();
	} else {
		return;
	}
	if (!svr->copyAttr("uptime", element) && element.isFloat()) {
		uptime = element.Float();
	} else {
		return;
	}

	status = VALID;

	if (!svr->copyAttr("entities", element) && element.isInt()) {
		entities = element.Int();
	}

	if (!svr->copyAttr("version", element) && element.isString()) {
		version = element.String();
	}

	if (!svr->copyAttr("builddate", element) && element.isString()) {
		buildDate = element.String();
	}

	if (!svr->copyAttr("protocol_version", element) && element.isInt()) {
		protocol_version = element.Int();
	}

	if (!svr->copyAttr("assets", element) && element.isList()) {
		auto assetsAsStrings = element.List() | std::views::filter([](auto entry) { return entry.isString(); }) | std::views::transform([](auto entry) { return entry.String(); });
		assets = {assetsAsStrings.begin(), assetsAsStrings.end()};
	}

}

} // of namespace Eris
