// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

#ifdef HAVE_CONFIG_H

#include "config.h"

#endif

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#include "common/net/CommHttpClient.h"
#include "CommPythonClient.h"
#include "CommMetaClient.h"
#include "CommMDNSPublisher.h"
#include "common/net/CommAsioListener_impl.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "EntityBuilder.h"
#include "Persistence.h"
#include "rules/simulation/WorldRouter.h"
#include "Ruleset.h"
#include "StorageManager.h"
#include "IdleConnector.h"
#include "PossessionAuthenticator.h"
#include "TrustedConnection.h"
#include "common/net/HttpHandling.h"

#include "pythonbase/Python_API.h"
#include "rules/simulation/LocatedEntity.h"
#include "rules/simulation/World.h"

#ifdef CYPHESIS_USE_POSTGRES

#include "common/CommPSQLSocket.h"
#include "common/DatabasePostgres.h"

#endif

#include "common/id.h"
#include "common/const.h"
#include "common/AtlasFactories.h"
#include "rules/simulation/Inheritance.h"
#include "common/system.h"
#include "common/sockets.h"
#include "common/Monitors.h"
#include "common/Variable.h"
#include "ExternalMindsManager.h"
#include "Player.h"
#include "ServerPropertyManager.h"
#include "TypeUpdateCoordinator.h"
#include "TeleportProperty.h"
#include "ScriptReloader.h"
#include "AccountProperty.h"
#include "Remotery.h"
#include "common/Storage.h"
#include "common/net/SquallHandler.h"
#include "SquallAssetsGenerator.h"
#include "AssetsHandler.h"


#include <common/FileSystemObserver.h>
#include <common/AssetsManager.h>
#include <common/DatabaseSQLite.h>
#include <common/RepeatedTask.h>
#include <common/MainLoop.h>
#include <rules/simulation/python/CyPy_Server.h>
#include <rules/python/CyPy_Physics.h>
#include <rules/entityfilter/python/CyPy_EntityFilter_impl.h>
#include <rules/python/CyPy_Atlas.h>
#include <rules/python/CyPy_Common.h>
#include <rules/python/CyPy_Rules_impl.h>
#include <rules/simulation/python/CyPy_LocatedEntity_impl.h>
#include <rules/simulation/ExternalMind.h>
#include <rules/simulation/PhysicalDomain.h>
#include "saf/saf.hpp"

#include <Atlas/Objects/RootEntity.h>

#include <varconf/config.h>
#include <filesystem>

#include <memory>
#include <thread>
#include <fstream>

using namespace boost::asio;

class TrustedConnection;

class Peer;


namespace {
const bool debug_flag = false;

INT_OPTION(http_port_num, 6780, CYPHESIS, "httpport",
		   "Network listen port for http connection to the server")

BOOL_OPTION(useMetaserver, true, CYPHESIS, "usemetaserver",
			"Flag to control registration with the metaserver")

STRING_OPTION(mserver, "metaserver.worldforge.org", CYPHESIS, "metaserver",
			  "Hostname to use as the metaserver")

INT_OPTION(ai_clients, 1, CYPHESIS, "aiclients",
		   "Number of AI clients to spawn.")

/**
 * Wraps either a Postgres server connection along with a vacuum socket, or a SQLite connection along with a vacuum task.
 */
struct ServerDatabase {
	virtual ~ServerDatabase() = default;

	virtual Database& database() = 0;

	virtual void stopVacuum() = 0;
};

#ifdef CYPHESIS_USE_POSTGRES

struct PgServerDatabase : public ServerDatabase
{
	std::unique_ptr<DatabasePostgres> m_db;
	std::unique_ptr<CommPSQLSocket> m_vacuumTask;

	PgServerDatabase(std::unique_ptr<DatabasePostgres> db,
					 std::unique_ptr<CommPSQLSocket> vacuumTask) :
			m_db(std::move(db)),
			m_vacuumTask(std::move(vacuumTask))
	{}

	~PgServerDatabase() override = default;

	Database& database() override
	{
		return *m_db;
	}

	void stopVacuum() override
	{
		m_vacuumTask->cancel();
	}
};

#endif

struct SQLiteServerDatabase : public ServerDatabase {
	std::unique_ptr<DatabaseSQLite> m_db;
	std::unique_ptr<RepeatedTask> m_vacuumTask;

	SQLiteServerDatabase(std::unique_ptr<DatabaseSQLite> db,
						 std::unique_ptr<RepeatedTask> vacuumTask) :
			m_db(std::move(db)),
			m_vacuumTask(std::move(vacuumTask)) {}

	~SQLiteServerDatabase() override {
		m_vacuumTask->cancel();
	}

	Database& database() override {
		return *m_db;
	}

	void stopVacuum() override {
		m_vacuumTask->cancel();
	}

};

std::unique_ptr<ServerDatabase> createDatabase(io_context& io_context) {
	std::string databaseBackend;
	readConfigItem(instance, "database", databaseBackend);

	if (databaseBackend == "postgres") {
#ifdef CYPHESIS_USE_POSTGRES
		auto database = new DatabasePostgres();
		return std::make_unique<PgServerDatabase>(std::unique_ptr<DatabasePostgres>(database), std::make_unique<CommPSQLSocket>(io_context, *database));
#else
		spdlog::error("Database specified as 'postgres', but this server is not built with Postgres SQL support.");
		throw std::runtime_error("Database specified as 'postgres', but this server is not built with Postgres SQL support.");
#endif
	} else {
		auto sqliteDatabase = new DatabaseSQLite();
		auto task = std::make_unique<RepeatedTask>(io_context, std::chrono::seconds(25 * 60), [=]() { sqliteDatabase->runMaintainance(); });
		return std::make_unique<SQLiteServerDatabase>(std::unique_ptr<DatabaseSQLite>(sqliteDatabase), std::move(task));
	}
}

std::unique_ptr<CommMetaClient> createMetaClient(io_context& io_context) {
	std::unique_ptr<CommMetaClient> cmc;
	if (useMetaserver) {
		spdlog::debug("Trying to connect to meta server at {}.", mserver);
		cmc = std::make_unique<CommMetaClient>(io_context);
		if (cmc->setup(mserver) != 0) {
			spdlog::error("Error creating meta server comm channel.");
			cmc.reset();
		}
	} else {
		spdlog::debug("Not trying to connect to meta server.");
	}
	return cmc;
}

std::unique_ptr<CommMDNSPublisher> createMDNSClient(io_context& io_context, ServerRouting& serverRouting) {
	std::unique_ptr<CommMDNSPublisher> cmdns;
#if defined(HAVE_AVAHI)
	cmdns = std::make_unique<CommMDNSPublisher>(io_context, serverRouting);
	if (cmdns->setup() != 0) {
		spdlog::error("Unable to register service with MDNS daemon.");
		cmdns.reset();
	}
#endif // defined(HAVE_AVAHI)
	return cmdns;
}

struct SocketListeners {
	std::list<CommAsioListener<ip::tcp, CommAsioClient<ip::tcp>>> tcp_atlas_clients;
	std::unique_ptr<CommAsioListener<local::stream_protocol, CommPythonClient>> pythonListener;
	std::unique_ptr<CommAsioListener<local::stream_protocol, CommAsioClient<local::stream_protocol>>> localListener;
	std::unique_ptr<CommAsioListener<ip::tcp, CommHttpClient>> httpListener;
};

SocketListeners createListeners(io_context& io_context, ServerRouting& serverRouting, Atlas::Objects::Factories& atlasFactories, HttpRequestProcessor& httpRequestProcessor) {

	SocketListeners socketListeners;

	auto tcpAtlasCreator = [&]() -> std::shared_ptr<CommAsioClient<ip::tcp>> {
		return std::make_shared<CommAsioClient<ip::tcp>>(serverRouting.getName(), io_context, atlasFactories);
	};

	std::function<void(CommAsioClient<ip::tcp>&)> tcpAtlasStarter = [&](CommAsioClient<ip::tcp>& client) {
		auto connection_id = newId();
		//Turn off Nagle's algorithm to increase responsiveness.
		client.getSocket().set_option(ip::tcp::no_delay(true));
		//Listen to both ipv4 and ipv6
		//client.getSocket().set_option(boost::asio::ip::v6_only(false));
		client.startAccept(std::make_unique<Connection>(client, serverRouting, "", connection_id));
	};


	if (client_port_num < 0) {
		client_port_num = dynamic_port_start;
		for (; client_port_num <= dynamic_port_end; client_port_num++) {
			try {
				socketListeners.tcp_atlas_clients.emplace_back(tcpAtlasCreator, tcpAtlasStarter, serverRouting.getName(), io_context,
															   ip::tcp::endpoint(ip::tcp::v6(), client_port_num));
			} catch (const std::exception& e) {
				break;
			}
		}
		if (client_port_num < dynamic_port_end) {
			auto errorMsg = fmt::format("Could not find free client listen "
										"socket in range {}-{}. Init failed.",
										dynamic_port_start, dynamic_port_end);
			spdlog::error(errorMsg);
			spdlog::info("To allocate 8 more ports please run:"
						 "\n\n    cyconfig "
						 "--cyphesis:dynamic_port_end={}\n\n",
						 dynamic_port_end + 8);
			throw std::runtime_error(errorMsg);
		}
		spdlog::info("Auto configuring new instance \"{}\" "
					 "to use port {}.", instance, client_port_num);
		global_conf->setItem(instance, "tcpport", client_port_num, varconf::USER);
		global_conf->setItem(CYPHESIS, "dynamic_port_start", client_port_num + 1, varconf::USER);
	} else {
		try {
			socketListeners.tcp_atlas_clients.emplace_back(tcpAtlasCreator,
														   tcpAtlasStarter,
														   serverRouting.getName(),
														   io_context,
														   ip::tcp::endpoint(ip::tcp::v6(), client_port_num));
		} catch (const std::exception& e) {
			auto errorMsg = fmt::format("Could not create client listen socket "
										"on port {}. Init failed. The most common reason for this "
										"is that you're already running an instance of Cyphesis."
										"\nError: {}",
										client_port_num, e.what());
			spdlog::error(errorMsg);
			throw std::runtime_error(errorMsg);
		}
	}

	remove(python_socket_name.c_str());
	auto pythonCreator = [&]() -> std::shared_ptr<CommPythonClient> {
		return std::make_shared<CommPythonClient>(serverRouting.getName(), io_context);
	};
	std::function<void(CommPythonClient&)> pythonStarter =
			[&](CommPythonClient& client) {
				client.startAccept();
			};
	socketListeners.pythonListener = std::make_unique<CommAsioListener<local::stream_protocol, CommPythonClient>>(pythonCreator,
																												  pythonStarter,
																												  serverRouting.getName(),
																												  io_context,
																												  local::stream_protocol::endpoint(python_socket_name));

	remove(client_socket_name.c_str());
	auto localCreator = [&]() -> std::shared_ptr<CommAsioClient<local::stream_protocol>> {
		return std::make_shared<CommAsioClient<local::stream_protocol>>(serverRouting.getName(), io_context, atlasFactories);
	};
	auto localStarter = [&](CommAsioClient<local::stream_protocol>& client) {
		auto connection_id = newId();
		client.startAccept(std::make_unique<TrustedConnection>(client, serverRouting, "", connection_id));
	};
	socketListeners.localListener = std::make_unique<CommAsioListener<local::stream_protocol, CommAsioClient<local::stream_protocol>>>(localCreator,
																																	   localStarter,
																																	   serverRouting.getName(),
																																	   io_context,
																																	   local::stream_protocol::endpoint(client_socket_name));
	spdlog::info("Listening to local named socket at {}", client_socket_name);


	auto httpCreator = [&]() -> std::shared_ptr<CommHttpClient> {
		return std::make_shared<CommHttpClient>(serverRouting.getName(), io_context, httpRequestProcessor);
	};

	auto httpStarter = [&](CommHttpClient& client) {
		//Listen to both ipv4 and ipv6
		//client.getSocket().set_option(boost::asio::ip::v6_only(false));
		client.serveRequest();
	};

	socketListeners.httpListener = std::make_unique<CommAsioListener<ip::tcp, CommHttpClient> >(httpCreator, httpStarter, serverRouting.getName(), io_context,
																								ip::tcp::endpoint(ip::tcp::v6(), http_port_num));

	spdlog::info("Http service. The following endpoints are available over port {}.", http_port_num);
	spdlog::info(" /config : shows server configuration");
	spdlog::info(" /monitors : various monitored values, suitable for time series systems");
	spdlog::info(" /monitors/numerics : only numerical values, suitable for time series system that only operates on numerical data");

	return socketListeners;
}


int run() {
	Remotery* rmt = nullptr;
	auto remoteryEnabled = global_conf->getItem(CYPHESIS, "remotery");
	if (remoteryEnabled.is_bool() && ((bool) remoteryEnabled)) {
		rmtSettings* settings = rmt_Settings();
		settings->reuse_open_port = true;
		auto error = rmt_CreateGlobalInstance(&rmt);
		if (RMT_ERROR_NONE != error) {
			spdlog::error("Error launching Remotery {}", std::to_string(error));
			return -1;
		} else {
			spdlog::info("Remotery enabled on port {}.", settings->port);
		}
	}


	Monitors monitors;
	monitors.watch("minds", std::make_unique<Variable<int>>(ExternalMind::s_numberOfMinds));
	monitors.watch("players", std::make_unique<Variable<int>>(Player::s_numberOfPlayers));
	monitors.watch("physic_processing_us", std::make_unique<Variable<int>>(PhysicalDomain::s_processTimeUs));


	//Check if we should spawn AI clients.
	if (ai_clients) {
		spdlog::info("Spawning {} AI client processes.", ai_clients);
		for (int i = 0; i < ai_clients; ++i) {
			auto pid = fork();
			if (pid == 0) {
				execl((bin_directory + "/cyaiclient").c_str(),
					  (bin_directory + "/cyaiclient").c_str(),
					  fmt::format("--cyphesis:confdir={}", etc_directory).c_str(),
					  fmt::format("--cyphesis:vardir={}", var_directory).c_str(),
					  fmt::format("--cyphesis:directory={}", share_directory).c_str(),
					  nullptr);
				return 0;
			} else if (pid == -1) {
				spdlog::warn("Could not spawn AI client process.");
			}
		}
	}

	//A pointer because we need to reset this before we shut down the Python API. Perhaps this could be done better?
	auto io_context = std::make_unique<boost::asio::io_context>();

	try {
		auto& atlasFactories = AtlasFactories::factories;

		// Initialise the persistence subsystem.
		auto serverDatabase = createDatabase(*io_context);
		auto& database = serverDatabase->database();

		//Creating a "Storage" object makes sure all database tables are setup correctly.
		Storage storage(database);
		Persistence persistence(database);

		// If the restricted flag is set in the config file, then we
		// don't allow connecting users to create accounts. Accounts must
		// be created manually by the server administrator.
		if (restricted_flag) {
			spdlog::info("Setting restricted mode.");
		}

		readConfigItem(instance, "inittime", timeoffset);

		std::string server_name;
		if (readConfigItem(instance, "servername", server_name) != 0) {
			if (instance == CYPHESIS) {
				server_name = get_hostname();
			} else {
				server_name = instance;
			}
		}

		int nice = 0;
		readConfigItem(instance, "nice", nice);


		AssetsManager assets_manager(std::make_unique<FileSystemObserver>(*io_context));

		std::filesystem::path squallRepositoryPath = std::filesystem::path(var_directory) / "lib" / "cyphesis" / "squall";
		AssetsHandler assetsHandler{squallRepositoryPath};

		assets_manager.observeAssetsDirectory();
		auto assetsPath = assets_manager.getAssetsPath();


		//Perhaps move all this into the AssetsManager?
		spdlog::info("Reading assets from {}", assetsPath.string());


		spdlog::info("Setting up Squall repository at {}, with assets located at {}.", squallRepositoryPath.string(), assetsPath.string());
		spdlog::info("Will now generate the Squall repository. This will take some time the first time the server is ran, but should be quick once done.");
		auto rootSignatureResult = assetsHandler.refreshSquallRepository(assetsPath);
		if (rootSignatureResult) {
			spdlog::info("Generated squall signature {}.", rootSignatureResult->str_view());
		} else {
			spdlog::warn("Could not generate Squall signature.");
			return 1;
		}


		std::vector<std::string> python_directories;
		// Add the path to the non-ruleset specific code.
		python_directories.push_back(share_directory + "/cyphesis/scripts");
		// Add the path to the ruleset specific code.
		python_directories.push_back(share_directory + "/cyphesis/rulesets/" + ruleset_name + "/scripts");

		// Start up the Python subsystem.
		init_python_api({&CyPy_Server::init,
						 &CyPy_Rules<LocatedEntity, CyPy_LocatedEntity>::init,
						 &CyPy_Physics::init,
						 &CyPy_EntityFilter<LocatedEntity>::init,
						 &CyPy_Atlas::init,
						 &CyPy_Common::init},
						python_directories);
		observe_python_directories(*io_context, assets_manager);

		Inheritance inheritance;
		ServerPropertyManager propertyManager(inheritance);

		EntityBuilder entityBuilder;

		Ruleset ruleset(entityBuilder, *io_context, propertyManager);
		ruleset.loadRules(ruleset_name);


		std::chrono::steady_clock::duration time{};
		auto timeProviderFn = [&]() -> std::chrono::steady_clock::duration { return time; };

		PossessionAuthenticator possessionAuthenticator;

		ExternalMindsManager externalMindsManager(possessionAuthenticator);

		Ref<LocatedEntity> baseEntity = new World();
		baseEntity->setType(inheritance.getType("world"));
		WorldRouter world(baseEntity, entityBuilder, timeProviderFn);
		baseEntity.reset();

		std::map<int, int> operationsMap;
		monitors.watch("operations_processed", std::make_unique<Variable<int>>(world.m_operationsCount));
		world.Dispatching.connect([&](const Operation& op) {
			auto I = operationsMap.find(op->getClassNo());
			if (I == operationsMap.end()) {
				auto result = operationsMap.emplace(op->getClassNo(), 1);
				monitors.watch(fmt::format("operation_count{{type=\"{}\"}}", op->getParent()), std::make_unique<Variable<int>>(result.first->second));
			} else {
				I->second++;
			}
		});

		CyPy_Server::registerWorld(&world);

		StorageManager store(world, serverDatabase->database(), entityBuilder, propertyManager);

		//Instantiate at startup
		HttpHandling httpCache(monitors);
		httpCache.mHandlers.emplace_back(buildSquallHandler(squallRepositoryPath / "data"));

		// This ID is currently generated every time, but should perhaps be
		// persistent in future.
		long lobby_int_id;

		if ((lobby_int_id = database.newId()) < 0) {
			spdlog::critical("Unable to get server IDs from Database");
			return EXIT_DATABASE_ERROR;
		}

		// Create the core serverRouting object, which stores central data,
		// and track objects
		ServerRouting serverRouting(world,
									persistence,
									ruleset_name,
									server_name,
									RouterId{lobby_int_id});
		serverRouting.setAssets({assetsHandler.resolveAssetsUrl()});


		auto& ctx = *io_context;
		assets_manager.observeDirectory(assetsPath.string(), [assetsPath, &ctx, &assetsHandler, &serverRouting](const std::filesystem::path& path) {

			spdlog::debug("Assets have changed; will regenerate Squall repository.");
			auto promise = saf::promise<std::optional<Squall::Signature>>{ctx};
			auto future = promise.get_future();
			std::async([assetsPath, p = std::move(promise), &assetsHandler]() mutable {
				auto result = assetsHandler.refreshSquallRepository(assetsPath);
				p.set_value(result);
			});

			boost::asio::co_spawn(ctx, [future = std::move(future), &assetsHandler, &serverRouting]() mutable -> boost::asio::awaitable<void> {
				auto result = co_await future.async_extract();
				if (result.has_value()) {
					spdlog::info("New Squall repository root is {}.", result.value().str());
					serverRouting.setAssets({assetsHandler.resolveAssetsUrl()});
				} else {
					spdlog::warn("Couldn't generate new Squall repository after file change.");
				}
			}, boost::asio::detached);

		});


		//Need to register the server routing instance with the Teleport property.
		TeleportProperty::s_serverRouting = &serverRouting;
		AccountProperty::s_serverRouting = &serverRouting;

		TypeUpdateCoordinator typeUpdateCoordinator(inheritance, world, serverRouting);

		ScriptReloader scriptReloader(world);

		run_user_scripts("cyphesis");

		spdlog::info("Restoring world from database...");

		store.restoreWorld(world.getBaseEntity());
		// Read the world entity if any from the database, or set it up.
		// If it was there, make sure it did not get any of the wrong
		// position or orientation data.
		store.initWorld(world.getBaseEntity());

		spdlog::info("Restored world.");

		// Configuration is now complete, and verified as somewhat sane, so
		// we save the updated user config.
		updateUserConfiguration();

		// Inform things that want to know that we are running.
		running();

		// Reduce our system priority to make it easier to debug a runaway
		// server.
		if (nice != 0) {
			reduce_priority(nice);
		}

		//Check if the world is populated; if it's not we should perhaps import some entities into it.
		//Note that we only check the top level; we don't perform a full hierarchical
		//traversal. Mainly because we never need to.
		auto autoImport = global_conf->getItem(ruleset_name, "autoimport");
		if (autoImport.is_string() && !autoImport.as_string().empty()) {
			std::string importPath = autoImport.as_string();
			std::ifstream file(importPath);
			if (file.good()) {
				file.close();
				//We should only try to import if the world isn't populated.
				bool isPopulated = false;
				if (world.getBaseEntity()->m_contains) {
					for (const auto& entity: *world.getBaseEntity()->m_contains) {
						//if there's any entity that's not transient we consider it populated
						if (!entity->hasAttr("transient")) {
							isPopulated = true;
							break;
						}
					}
				}

				if (!isPopulated) {
					//Populate the server through separate process (mainly because it's easier as we've
					//already written the importer tool; we might also do it in-process, but that would
					//require some rewriting of code).
					spdlog::info("Trying to import world from {}.", importPath);
					std::stringstream ss;
					ss << bin_directory << "/cyimport";
					ss << " --cyphesis:confdir=\"" << etc_directory << "\"";
					ss << " --cyphesis:vardir=\"" << var_directory << "\"";
					ss << " --cyphesis:directory=\"" << share_directory << "\"";
					ss << " --resume \"" << importPath + "\"";
					std::string command = ss.str();
					std::thread importer([=]() {
						int result = std::system(command.c_str());
						if (result == 0) {
							spdlog::info("Imported world into empty server.");
						} else {
							spdlog::info("No world imported.");
						}
					});
					importer.detach();
				}
			} else {
				spdlog::warn("Not importing as \"{}\" could not be found.", importPath);
				file.close();
			}
		}

		auto softExitStart = [&]() {
			return std::chrono::seconds(1);
		};


		auto softExitPoll = [&]() {
			return true;
		};

		auto softExitTimeout = [&]() {
		};

		auto dispatchOperationsFn = [&]() {
			serverRouting.dispatch(5);
		};


		//Initially there are a couple of pent-up operations we need to run to get up to speed. 10 seconds is a suitable large number.
		world.getOperationsHandler().processUntil(time, std::chrono::seconds(10));
		//Report to the log when time diff between when an operation should have been handled and when it actually was
		world.getOperationsHandler().m_time_diff_report = std::chrono::milliseconds(200);

		//Inner loop, where listeners are active.
		{
			auto socketListeners = createListeners(*io_context, serverRouting, atlasFactories, httpCache);

			auto metaClient = createMetaClient(*io_context);
			auto mdnsClient = createMDNSClient(*io_context, serverRouting);

			IdleConnector storage_idle(*io_context);
			storage_idle.idling.connect([&store]() { store.tick(); });

			spdlog::info("Running and accepting connections");
			logEvent(START, "- - - Standalone server startup");

			MainLoop::run(daemon_flag, *io_context, world.getOperationsHandler(), {softExitStart, softExitPoll, softExitTimeout, dispatchOperationsFn}, time);
			if (metaClient) {
				metaClient->metaserverTerminate();
			}
			assets_manager.stopFileObserver();
			//Cancel vacuum tasks, otherwise they might hold up the io_context.
			serverDatabase->stopVacuum();
		}

		//Actually, there's no way for the world to know that it's shutting down,
		//as the shutdown signal most probably comes from a sighandler. We need to
		//tell it it is shutting down so that it can do some housekeeping.
		try {
			exit_flag = false;
			if (store.shutdown(exit_flag, world.getEntities()) != 0) {
				//Ignore this error and carry on with shutting down.
				spdlog::error("Error when shutting down");
			}
		} catch (const std::exception& e) {
			spdlog::error("Exception caught when shutting down: {}", e.what());
		} catch (...) {
			//Ignore this error and carry on with shutting down.
			spdlog::error("Exception caught when shutting down");
		}


		serverRouting.disconnectAllConnections();

		//Clear out reference
		world.shutdown();

		//Run outstanding tasks from the shut down connections and listeners.
		io_context->run();

	} catch (const std::exception& ex) {
		std::cerr << "There was an error when running the server, will shut down: " << ex.what() << std::endl;
	}
	//Run any outstanding tasks before shutting down service.
	io_context->run();

	io_context.reset();

	shutdown_python_api();

	//The global_conf instance is created at loadConfig(...), so we'll destroy it here
	delete global_conf;
	if (rmt) {
		rmt_DestroyGlobalInstance(rmt);
	}

	return 0;
}
}

int main(int argc, char** argv) {
	std::setlocale(LC_ALL, "C");
	//TODO: read from config instead
	spdlog::set_level(spdlog::level::debug);
	if (security_init() != 0) {
		spdlog::critical("Security initialization Error. Exiting.");
		return EXIT_SECURITY_ERROR;
	}

	if (security_check() != SECURITY_OKAY) {
		spdlog::critical("Security check error. Exiting.");
		return EXIT_SECURITY_ERROR;
	}

	//Turn on soft exits so we get a chance to persist external clients' thoughts.
	exit_soft_enabled = true;

	interactive_signals();

	int config_status = loadConfig(argc, argv, USAGE_SERVER);
	if (config_status < 0) {
		if (config_status == CONFIG_VERSION) {
			std::cout << argv[0] << " (cyphesis) " << consts::version
					  << " (Cyphesis build: " << consts::buildId << ")"
					  << std::endl;

			return 0;
		} else if (config_status == CONFIG_HELP) {
			showUsage(argv[0], USAGE_SERVER);
			return 0;
		} else if (config_status != CONFIG_ERROR) {
			spdlog::error("Unknown error reading configuration.");
		}
		// Fatal error loading config file.
		return EXIT_CONFIG_ERROR;
	}

	if (daemon_flag) {
		int pid = daemonise();
		if (pid == -1) {
			return EXIT_FORK_ERROR;
		} else if (pid > 0) {
			return EXIT_SUCCESS;
		}
	}


	auto result = run();
	if (result != 0) {
		return result;
	}
	spdlog::info("Clean shutdown complete.");
	logEvent(STOP, "- - - Standalone server shutdown");

	return result;
}
