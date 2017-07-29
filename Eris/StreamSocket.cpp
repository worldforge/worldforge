/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "StreamSocket.h"
#include "Log.h"

#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/objectFactory.h>

using namespace boost::asio;

static const int NEGOTIATE_TIMEOUT_SECONDS = 5;

namespace Eris
{

StreamSocket::StreamSocket(io_service& io_service,
        const std::string& client_name, Atlas::Bridge& bridge,
        Callbacks& callbacks) :
        m_io_service(io_service), _bridge(bridge), _callbacks(callbacks),
        mWriteBuffer(new boost::asio::streambuf()), mSendBuffer(new boost::asio::streambuf()),
        mInStream(&mReadBuffer), mOutStream(mWriteBuffer), mShouldSend(false), mIsSending(false),
        _sc(new Atlas::Net::StreamConnect(client_name, mInStream, mOutStream)),
        _negotiateTimer(io_service), _connectTimer(io_service), m_codec(nullptr), m_encoder(nullptr), m_is_connected(false)
{
}
StreamSocket::~StreamSocket()
{
    delete _sc;
    delete mWriteBuffer;
    delete mSendBuffer;
    delete m_encoder;
    delete m_codec;
}

void StreamSocket::detach()
{
    _callbacks = Callbacks();
}

void StreamSocket::startNegotiation()
{
    auto self(this->shared_from_this());
    _negotiateTimer.expires_from_now(
            boost::posix_time::seconds(NEGOTIATE_TIMEOUT_SECONDS));
    _negotiateTimer.async_wait([this, self](const boost::system::error_code& ec)
    {
        //If the negotiator still exists after the deadline it means that the negotation hasn't
        //completed yet; we'll consider that a "timeout".
            if (_sc != nullptr) {
                if (_callbacks.stateChanged) {
                    debug() << "Client disconnected because of negotiation timeout.";
//                log(NOTICE, "Client disconnected because of negotiation timeout.");
                _callbacks.stateChanged(DISCONNECTING);
//                mSocket.close();
                }
            }
        });
    _callbacks.stateChanged(NEGOTIATE);

    _sc->poll(false);

    write();
    negotiate_read();
}

Atlas::Negotiate::State StreamSocket::negotiate()
{
    // poll and check if negotiation is complete
    _sc->poll();

    if (_sc->getState() == Atlas::Negotiate::IN_PROGRESS) {
        return _sc->getState();
    }

    // Check if negotiation failed
    if (_sc->getState() == Atlas::Negotiate::FAILED) {
        return _sc->getState();
    }
    // Negotiation was successful

    _negotiateTimer.cancel();

    // Get the codec that negotiation established
    m_codec = _sc->getCodec(_bridge);

    // Acceptor is now finished with
    delete _sc;
    _sc = 0;

    if (m_codec == nullptr) {
        error() << "Could not create codec during negotiation.";
        return Atlas::Negotiate::FAILED;
    }
    // Create a new encoder to send high level objects to the codec
    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);

    // This should always be sent at the beginning of a session
    m_codec->streamBegin();

    _callbacks.stateChanged(CONNECTED);

    return Atlas::Negotiate::SUCCEEDED;
}

Atlas::Codec& StreamSocket::getCodec()
{
    assert(m_codec);
    return *m_codec;
}

Atlas::Objects::ObjectsEncoder& StreamSocket::getEncoder()
{
    assert(m_encoder);
    return *m_encoder;
}

}
