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

#include <Eris/BaseConnection.h>
#include <Eris/Exceptions.h>
#include <Eris/Log.h>

#include "SignalFlagger.h"

#include <Atlas/Codecs/XML.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Objects/objectFactory.h>
#include <Atlas/Objects/Encoder.h>

#include <skstream/skstream.h>

#include <cstdlib>

#include <cassert>

class TestBaseConnection : public Eris::BaseConnection {
  public:
    bool failure;
    bool timeout;

    TestBaseConnection(Atlas::Bridge * b) : Eris::BaseConnection("test", "1", b), failure(false), timeout(false) { }

    virtual void handleFailure(const std::string & msg) {
        failure = true;
    }

    virtual void handleTimeout(const std::string & msg) {
        timeout = true;
    }

    void test_setStatus(Eris::BaseConnection::Status sc) {
        setStatus(sc);
    }

    void test_onConnect() {
        onConnect();
    }

    void test_hardDisconnect(bool flag) {
        hardDisconnect(flag);
    }

    void test_onConnectTimeout() {
        onConnectTimeout();
    }

    void test_onNegotiateTimeout() {
        onNegotiateTimeout();
    }

    void setup_stream() {
        _stream = new tcp_socket_stream;
    }

    void setup_codec() {
        m_codec = new Atlas::Codecs::XML(*_stream, *_bridge);
    }

    void setup_encode() {
        _encode = new Atlas::Objects::ObjectsEncoder(*m_codec);
    }

    void setup_sc() {
        _sc = new Atlas::Net::StreamConnect(_clientName, *_stream);
    }
};

static void writeLog(Eris::LogLevel, const std::string & msg)
{
    std::cerr << msg << std::endl << std::flush;
}

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));

    Atlas::Objects::Factories * f = Atlas::Objects::Factories::instance();
    assert(!f->hasFactory("unseen"));

    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);
    }

    // Make sure the op classes have been installed, and the initial
    // constructor code path has been tested.
    assert(f->hasFactory("unseen"));
    assert(f->hasFactory("attack"));

    // Test the other code path when a second connection is created.
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);
    }

    // Test isConnected.
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        assert(!tbc.isConnected());
    }

    // Test getFileDescriptor.
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        try {
            tbc.getFileDescriptor();

            std::cerr << "FAIL: BaseConnection::getFileDescriptor() should throw Eris::InvalidOperation" << std::endl << "FAIL: when not coonected." << std::endl << std::flush;
            abort();
        }
        catch (Eris::InvalidOperation & eio) {
        }
    }

    // Test getStatus().
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        assert(tbc.getStatus() == Eris::BaseConnection::DISCONNECTED);
    }

    // Test setStatus().
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        assert(tbc.getStatus() == Eris::BaseConnection::DISCONNECTED);

        tbc.test_setStatus(Eris::BaseConnection::CONNECTED);

        assert(tbc.getStatus() == Eris::BaseConnection::CONNECTED);

        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test alternate path through desctructor using setStatus()
    {
        TestBaseConnection * tbc = new TestBaseConnection(new Atlas::Message::QueuedDecoder);

        assert(tbc->getStatus() == Eris::BaseConnection::DISCONNECTED);

        tbc->setup_stream();
        tbc->setup_codec();
        tbc->setup_encode();
        tbc->test_setStatus(Eris::BaseConnection::CONNECTED);

        // The destructor will throw in hardDisconnect();
        try {
            delete tbc;
            abort();
        }
        catch (Eris::InvalidOperation & eio) {
        }
    }

    // Test connect() and verify getStatus() changes.
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        assert(tbc.getStatus() == Eris::BaseConnection::DISCONNECTED);

        int ret = tbc.connect("localhost", 6723);

        assert(ret == 0);

        assert(tbc.getStatus() == Eris::BaseConnection::CONNECTING);
    }

    // Test onConnect() does nothing.
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        tbc.test_onConnect();
    }

    // Test onConnect() emits the signal.
    {
        SignalFlagger onConnect_checker;

        assert(!onConnect_checker.flagged());

        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        tbc.Connected.connect(sigc::mem_fun(onConnect_checker,
                                            &SignalFlagger::set));

        assert(!onConnect_checker.flagged());

        tbc.test_onConnect();

        assert(onConnect_checker.flagged());
    }

    // Test hardDisconnect() does nothing.
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        tbc.test_hardDisconnect(true);
    }

    // Test hardDisconnect() throws in polldefault when connected
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        // Add members to be consistent with connected state.
        tbc.setup_stream();
        tbc.setup_codec();
        tbc.setup_encode();
        // Make the state different
        tbc.test_setStatus(Eris::BaseConnection::CONNECTED);

        try {
            tbc.test_hardDisconnect(true);
        }
        catch (Eris::InvalidOperation & eio) {
        }

        // Make it disconnected again, or we crash on destructor
        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test hardDisconnect() throws in polldefault when disconnecting
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        // Add members to be consistent with disconnecting state.
        tbc.setup_stream();
        tbc.setup_codec();
        tbc.setup_encode();
        // Make the state different
        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTING);

        try {
            tbc.test_hardDisconnect(true);
        }
        catch (Eris::InvalidOperation & eio) {
        }

        // Make it disconnected again, or we crash on destructor
        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test hardDisconnect() throws in polldefault when negotiating
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        // Add members to be consistent with negotiating state.
        tbc.setup_stream();
        tbc.setup_sc();
        // Make the state different
        tbc.test_setStatus(Eris::BaseConnection::NEGOTIATE);

        try {
            tbc.test_hardDisconnect(true);
        }
        catch (Eris::InvalidOperation & eio) {
        }

        // Make it disconnected again, or we crash on destructor
        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test hardDisconnect() throws in polldefault when negotiating
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        // Add members to be consistent with connecting state.
        tbc.setup_stream();
        // Make the state different
        tbc.test_setStatus(Eris::BaseConnection::CONNECTING);

        try {
            tbc.test_hardDisconnect(true);
        }
        catch (Eris::InvalidOperation & eio) {
        }

        // Make it disconnected again, or we crash on destructor
        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test hardDisconnect() throws in polldefault when negotiating
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        // Add members to be consistent with connecting state.
        tbc.setup_stream();
        // Make the state different
        tbc.test_setStatus(Eris::BaseConnection::INVALID_STATUS);

        try {
            tbc.test_hardDisconnect(true);
        }
        catch (Eris::InvalidOperation & eio) {
        }

        // Make it disconnected again, or we crash on destructor
        tbc.test_setStatus(Eris::BaseConnection::DISCONNECTED);
    }

    // Test onConnectTimeout()
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        tbc.test_onConnectTimeout();

        assert(tbc.timeout);
    }

    // Test onNegotiateTimeout()
    {
        TestBaseConnection tbc(new Atlas::Message::QueuedDecoder);

        tbc.test_onNegotiateTimeout();

        assert(tbc.timeout);
    }

    return 0;
}
