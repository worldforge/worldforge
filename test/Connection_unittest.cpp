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

#include <Eris/Connection.h>

#include <Eris/Log.h>
#include <Eris/Poll.h>

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

static void writeLog(Eris::LogLevel, const std::string & msg)
{       
    std::cerr << msg << std::endl << std::flush;
}

class TestConnection : public Eris::Connection {
  public:
    TestConnection(const std::string &cnm, const std::string& host, short port, bool dbg) : Eris::Connection(cnm, host, port, dbg) {
    }

    void testSetStatus(Status sc) { setStatus(sc); }

    void testGotData(Eris::PollData & data) { gotData(data); }
};

class TestPollData : public Eris::PollData {
    virtual bool isReady(const basic_socket*) { return false; }
};

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    // Test constructor
    {
        new Eris::Connection("eristest", "localhost", 6767, true);
    }

    // Test destructor
    {
        delete new Eris::Connection("eristest", "localhost", 6767, true);
    }

    // Test getTypeService()
    {
        Eris::Connection c("eristest", "localhost", 6767, true);

        assert(c.getTypeService() != 0);
    }

    // Test connect()
    {
        Eris::Connection c("eristest", "localhost", 6767, true);
        
        int ret = c.connect();

        assert(ret == 0);
    }

    // Test disconnect() when disconnected
    {
        TestConnection c("eristest", "localhost", 6767, true);

        c.disconnect();
    }

    // Test disconnect() when disconnecting
    {
        TestConnection c("eristest", "localhost", 6767, true);

        c.testSetStatus(Eris::BaseConnection::DISCONNECTING);

        c.disconnect();

        c.testSetStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test disconnect() when connecting
    {
        TestConnection c("eristest", "localhost", 6767, true);

        c.connect();

        assert(c.getStatus() == Eris::BaseConnection::CONNECTING);

        c.disconnect();

        c.testSetStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test gotData()
    {
        TestConnection c("eristest", "localhost", 6767, true);

        TestPollData data;

        c.testGotData(data);
    }

    // FIXME Not testing all the code paths through gotData()

    // Test send()
    {
        TestConnection c("eristest", "localhost", 6767, true);

        Atlas::Objects::Root obj;

        c.send(obj);
    }
    return 0;
}
