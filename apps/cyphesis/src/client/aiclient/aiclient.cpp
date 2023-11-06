// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

#include "PossessionClient.h"
#include "pythonbase/Python_API.h"
#include "rules/python/PythonScriptFactory.h"
#include "rules/ai/AwareMindFactory.h"
#include "rules/python/CyPy_Atlas.h"
#include "rules/python/CyPy_Common.h"
#include "rules/python/CyPy_Physics.h"
#include "rules/ai/python/CyPy_Ai.h"
#include "rules/entityfilter/python/CyPy_EntityFilter.h"

#include "common/sockets.h"
#include "common/RuleTraversalTask.h"
#include "common/Inheritance.h"

#define _GLIBCXX_USE_NANOSLEEP 1

#include "common/MainLoop.h"
#include "common/CommAsioClient.h"
#include "common/CommAsioClient_impl.h"
#include "common/AssetsManager.h"
#include "common/FileSystemObserver.h"
#include "common/operations/Think.h"
#include "common/globals.h"
#include "client/ClientPropertyManager.h"

#include <sys/prctl.h>
#include "rules/python/CyPy_Rules.h"
#include "rules/SimpleTypeStore.h"
#include "common/net/HttpHandling.h"
#include "common/net/CommAsioListener_impl.h"
#include "common/Variable.h"
#include "PossessionAccount.h"
#include <varconf/config.h>

#include <memory>
#include <utility>


using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

Atlas::Objects::Factories factories;

namespace {

    INT_OPTION(http_port_num, 6790, CYPHESIS, "httpport",
               "Network listen port for http connection to the client")

    void usage(const char* prgname)
    {
        std::cout << "usage: " << prgname << " [ local_socket_path ]" << std::endl;
    }

    void connectToServer(boost::asio::io_context& io_context, AwareMindFactory& mindFactory)
    {
        if (exit_flag_soft || exit_flag) {
            return;
        }
        auto commClient = std::make_shared<CommAsioClient<boost::asio::local::stream_protocol>>("aiclient", io_context, factories);

        commClient->getSocket().async_connect({client_socket_name}, [&io_context, &mindFactory, commClient](boost::system::error_code ec) {
            if (!ec) {
                spdlog::info("Connection detected; creating possession client.");

                /**
                 * The Flusher will flush the comm socket every ten milliseconds, as long as the connection is alive.
                 */
                struct Flusher : public std::enable_shared_from_this<Flusher>
                {
                    boost::asio::io_context& io_context;
                    std::weak_ptr<CommAsioClient<boost::asio::local::stream_protocol>> commClient;
                    boost::asio::steady_timer timer{io_context};
                    std::chrono::steady_clock::duration flushInterval;

                    Flusher(boost::asio::io_context& _io_context,
                            std::weak_ptr<CommAsioClient<boost::asio::local::stream_protocol>> _commClient,
                            std::chrono::steady_clock::duration _flushInterval)
                            : io_context(_io_context),
                              commClient(std::move(_commClient)),
                              flushInterval(_flushInterval)
                    {
                    }

                    void flush()
                    {
                        if (auto client = commClient.lock()) {
                            if (client->m_active && client->getSocket().is_open()) {
                                rmt_ScopedCPUSample(Flusher_flush, 0)
                                client->flush();
#if BOOST_VERSION >= 106600
                                timer.expires_after(flushInterval);
#else
                                timer.expires_from_now(flushInterval);
#endif
                                auto self(this->shared_from_this());
                                auto waitFn = [self](boost::system::error_code ecInner) {
                                    if (!ecInner) {
                                        self->flush();
                                    }
                                };
                                timer.async_wait(waitFn);
                            }
                        }
                    }
                };
                auto flusher = std::make_shared<Flusher>(io_context, commClient, std::chrono::milliseconds(10));
                flusher->flush();


                commClient->startConnect(std::make_unique<PossessionClient>(*commClient, mindFactory, std::make_unique<Inheritance>(factories), [&]() {
                    connectToServer(io_context, mindFactory);
                }));
            } else {
                //If we couldn't connect we'll wait five seconds and try again.
                auto timer = std::make_shared<boost::asio::steady_timer>(io_context);
#if BOOST_VERSION >= 106600
                timer->expires_after(std::chrono::seconds(5));
#else
                timer->expires_from_now(std::chrono::seconds(5));
#endif
                timer->async_wait([&io_context, &mindFactory, timer](boost::system::error_code ecInner) {
                    if (!ecInner) {
                        connectToServer(io_context, mindFactory);
                    }
                });
            }
        });
    }
}


STRING_OPTION(server, "localhost", "aiclient", "serverhost", "Hostname of the server to connect to")

STRING_OPTION(account, "", "aiclient", "account", "Account name to use to authenticate to the server")

STRING_OPTION(password, "", "aiclient", "password", "Password to use to authenticate to the server")


int main(int argc, char** argv)
{
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [AI] [%l] %v");

    //Kill ourselves if our parent is killed.
    prctl(PR_SET_PDEATHSIG, SIGTERM);

    Monitors monitors;

	//Perhaps tell spdlog to use a prefix?
    //setLoggingPrefix("AI");

    int config_status = loadConfig(argc, argv, USAGE_AICLIENT);
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_AICLIENT, "[ local_socket_path ]");
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
    } else if (optindex != argc) {
        usage(argv[0]);
        return 1;
    }


    Remotery* rmt = nullptr;
    auto remoteryEnabled = global_conf->getItem(CYPHESIS, "remotery");
    if (remoteryEnabled.is_bool() && ((bool) remoteryEnabled)) {
        rmtSettings* settings = rmt_Settings();
        settings->port = 17816; //Use a different port than default.
        settings->reuse_open_port = true;
        auto error = rmt_CreateGlobalInstance(&rmt);
        if (RMT_ERROR_NONE != error) {
            printf("Error launching Remotery %d\n", error);
            return -1;
        } else {
            spdlog::info("Remotery enabled on port {}.", settings->port);
        }
    }


    {
        ClientPropertyManager propertyManager{};
        SimpleTypeStore typeStore(propertyManager);

        {
            boost::asio::io_context io_context;
            HttpHandling httpCache(monitors);
            FileSystemObserver file_system_observer(io_context);
            AwareMindFactory mindFactory(typeStore);

            AssetsManager assets_manager(file_system_observer);
            assets_manager.init();


            std::vector<std::string> python_directories;
            // Add the path to the non-ruleset specific code.
            python_directories.push_back(share_directory + "/cyphesis/scripts");
            python_directories.push_back(share_directory + "/cyphesis/rulesets/basic/scripts");

            init_python_api({&CyPy_Ai::init,
                             &CyPy_Rules::init,
                             &CyPy_Physics::init,
                             &CyPy_EntityFilter::init,
                             &CyPy_Atlas::init,
                             &CyPy_Common::init},
                            std::move(python_directories), true);
            observe_python_directories(io_context, assets_manager);


            run_user_scripts("cyaiclient");


            //TODO: perhaps don't hardcode this; instead allowing for different classes for different minds?
            std::string script_package = "mind.NPCMind";
            std::string script_class = "NPCMind";

            if (mindFactory.m_scriptFactory != nullptr) {
                if (mindFactory.m_scriptFactory->package() != script_package) {
                    mindFactory.m_scriptFactory.reset();
                }
            }
            if (mindFactory.m_scriptFactory == nullptr) {
                auto psf = std::make_unique<PythonScriptFactory<BaseMind>>(script_package, script_class);
                if (psf->setup() == 0) {
                    spdlog::info("Initialized mind code with Python class {}.{}.", script_package, script_class);
                    mindFactory.m_scriptFactory = std::move(psf);
                } else {
                    spdlog::error("Python class \"{}.{}\" failed to load", script_package, script_class);
                }
            }

            boost::asio::signal_set signalSet(io_context);
            //If we're not running as a daemon we should use the interactive signal handler.
            if (!daemon_flag) {
                signalSet.add(SIGINT);
                signalSet.add(SIGTERM);
                signalSet.add(SIGHUP);
                signalSet.add(SIGQUIT);

            } else {
                signalSet.add(SIGTERM);
            }
            signalSet.async_wait([&](boost::system::error_code ec, int signal) {
                if (!ec) {
                    exit_flag = true;
                    exit_flag_soft = true;
                    io_context.stop();
                }
            });


            auto httpCreator = [&]() -> std::shared_ptr<CommHttpClient> {
                return std::make_shared<CommHttpClient>("cyaiclient", io_context, httpCache);
            };

            auto httpStarter = [&](CommHttpClient& client) {
                //Listen to both ipv4 and ipv6
                //client.getSocket().set_option(boost::asio::ip::v6_only(false));
                client.serveRequest();
            };

            CommAsioListener<boost::asio::ip::tcp, CommHttpClient> httpListener(httpCreator, httpStarter, "cyaiclient", io_context,
                                                                                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), http_port_num));

            spdlog::info("Http service. The following endpoints are available over port {}.", http_port_num);
            spdlog::info(" /config : shows client configuration");
            spdlog::info(" /monitors : various monitored values, suitable for time series systems");
            spdlog::info(" /monitors/numerics : only numerical values, suitable for time series system that only operates on numerical data");

            monitors.watch("accounts", PossessionAccount::account_count);
            monitors.watch("minds", PossessionAccount::mind_count);
            monitors.watch("operations_in", PossessionClient::operations_in);
            monitors.watch("operations_out", PossessionClient::operations_out);


            //Reload the script factory when scripts changes.
            //Any PossessionAccount instance will also take care of reloading the script instances.
            python_reload_scripts.connect([&]() {
                rmt_ScopedCPUSample(python_reload, 0)
                mindFactory.m_scriptFactory->refreshClass();
            });

            spdlog::info("Trying to connect to server at {}.", client_socket_name);
            connectToServer(io_context, mindFactory);

            /// \brief Use a "work" instance to make sure the io_context never runs out of work and is stopped.
            boost::asio::io_context::work m_io_work(io_context);

            io_context.run();

            signalSet.clear();

            spdlog::info("Shutting down.");
        }
        //We need to collect any objects before we delete the typeStore.
        PyGC_Collect();
    }
    shutdown_python_api();
    if (rmt) {
        rmt_DestroyGlobalInstance(rmt);
    }
}
