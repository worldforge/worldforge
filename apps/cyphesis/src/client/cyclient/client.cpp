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


#include "Python_ClientAPI.h"
#include "client/ClientPropertyManager.h"

#include "pythonbase/Python_API.h"

#include "common/globals.h"
#include "common/log.h"
#include "ObserverClient.h"

#include <varconf/config.h>

#include <cassert>
#include <rules/python/CyPy_Physics.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Common.h>
#include <rules/entityfilter/python/CyPy_EntityFilter_impl.h>
#include "rules/python/CyPy_Rules_impl.h"
#include "common/Property_impl.h"
#include "client/SimpleTypeStore.h"
#include "rules/ai/python/CyPy_MemEntity.h"

static void usage(const char* prgname) {
	std::cout << "usage: " << prgname << " [ [package.]function ]"
			  << std::endl;
}

STRING_OPTION(serverOption, "localhost", "client", "serverhost",
			  "Hostname of the server to connect to");

STRING_OPTION(accountOption, "", "client", "account",
			  "Account name to use to authenticate to the server");

STRING_OPTION(passwordOption, "", "client", "password",
			  "Password to use to authenticate to the server");

STRING_OPTION(packageOption, "", "client", "package",
			  "Python package which contains the world initialisation code");

STRING_OPTION(functionOption, "", "client", "function",
			  "Python function to initialise the world");

int main(int argc, char** argv) {
	int config_status = loadConfig(argc, argv, USAGE_CLIENT);
	if (config_status < 0) {
		if (config_status == CONFIG_VERSION) {
			reportVersion(argv[0]);
			return 0;
		} else if (config_status == CONFIG_HELP) {
			showUsage(argv[0], USAGE_CLIENT, "[ [package.]function]");
			return 0;
		} else if (config_status != CONFIG_ERROR) {
			spdlog::error("Unknown error reading configuration.");
		}
		// Fatal error loading config file
		return 1;
	}

	int optindex = config_status;

	assert(optindex <= argc);

	if (optindex == (argc - 1)) {
		std::string arg(argv[optindex]);
		std::string::size_type pos = arg.rfind('.');
		if (pos == std::string::npos) {
			// std::cout << "function " << arg << std::endl;
			functionOption = arg;
		} else {
			packageOption = arg.substr(0, pos);
			functionOption = arg.substr(pos + 1);
			// std::cout << "module.function " << package << "." << function << std::endl;
		}
	} else if (optindex != argc) {
		usage(argv[0]);
		return 1;
	}

	bool interactive = global_conf->findItem("", "interactive");
	int status = 0;

	std::vector<std::string> python_directories;
	// Add the path to the non-ruleset specific code.
	python_directories.push_back(share_directory + "/cyphesis/scripts");
	python_directories.push_back(share_directory + "/cyphesis/rulesets/basic/scripts");
	// Add the path to the ruleset specific code.
	python_directories.push_back(share_directory + "/cyphesis/rulesets/" + ruleset_name + "/scripts");

	init_python_api({&CyPy_Physics::init,
					 &CyPy_Rules<MemEntity, CyPy_MemEntity>::init,
					 &CyPy_EntityFilter<MemEntity>::init,
					 &CyPy_Atlas::init,
					 &CyPy_Common::init},
					python_directories, false);

	extend_client_python_api();
	ClientPropertyManager clientPropertyManager;
	SimpleTypeStore typeStore(clientPropertyManager);

	if (interactive) {
		python_prompt();
	} else {
		boost::asio::io_context io_context;
		std::map<std::string, std::string> keywords;
		Atlas::Objects::Factories factories;
		ObserverClient observerClient(io_context, factories, typeStore);
		observerClient.setup(accountOption, passwordOption, serverOption);
		python_client_script(packageOption, functionOption, observerClient);
	}

	shutdown_python_api();

	return status;
}
