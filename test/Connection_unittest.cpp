// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Eris/Connection.h>

#include <Eris/Log.h>
#include <Eris/EventService.h>

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

static void writeLog(Eris::LogLevel, const std::string & msg)
{       
    std::cerr << msg << std::endl << std::flush;
}

class TestConnection : public Eris::Connection {
  public:
    TestConnection(boost::asio::io_service& io_service, 
    		Eris::EventService& eventService, 
    		const Atlas::Objects::Factories& factories,
    		const std::string &cnm, 
    		const std::string& host,
    		short port) 
    : Eris::Connection(io_service,
    		eventService, 
    		factories,
    		cnm, 
    		host
    		, port) {
    }

    void testSetStatus(Status sc) { setStatus(sc); }

    void testDispatch() { dispatch(); }
};

int main()
{
	Atlas::Objects::Factories factories;
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    // Test constructor and destructor
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        Eris::Connection c(io_service, event_service, factories, "name", "localhost", 6767);
    }

    // Test getTypeService()
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        Eris::Connection c(io_service, event_service, factories, " name", "localhost", 6767);

        assert(c.getTypeService() != 0);
    }

    // Test connect()
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        Eris::Connection c(io_service, event_service, factories, " name", "localhost", 6767);
        
        int ret = c.connect();

        assert(ret == 0);
    }

    // Test connect() with socket
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        Eris::Connection c(io_service, event_service, factories, " name", "localhost", 6767);

        int ret = c.connect();

        assert(ret == 0);
    }

    // Test disconnect() when disconnected
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        Eris::Connection c(io_service, event_service, factories, " name", "localhost", 6767);

        c.disconnect();
    }

    // Test disconnect() when disconnecting
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        TestConnection c(io_service, event_service, factories, " name", "localhost", 6767);

        c.testSetStatus(Eris::BaseConnection::DISCONNECTING);

        c.disconnect();

        c.testSetStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test disconnect() when connecting
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        TestConnection c(io_service, event_service, factories, " name", "localhost", 6767);

        c.connect();

        assert(c.getStatus() == Eris::BaseConnection::CONNECTING);

        c.disconnect();

        c.testSetStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test dispatch()
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        TestConnection c(io_service, event_service, factories, " name", "localhost", 6767);

        c.testDispatch();
    }

    // FIXME Not testing all the code paths through gotData()

    // Test send()
    {
        boost::asio::io_service io_service;
        Eris::EventService event_service(io_service);
        Eris::Connection c(io_service, event_service, factories, " name", "localhost", 6767);

        Atlas::Objects::Root obj;

        c.send(obj);
    }
    return 0;
}
