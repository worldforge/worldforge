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
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef STREAMSOCKET_IMPL_H_
#define STREAMSOCKET_IMPL_H_


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "StreamSocket.h"

#include <Atlas/Codec.h>

#include <boost/date_time/posix_time/posix_time.hpp>

static const int CONNECT_TIMEOUT_SECONDS = 5;

namespace Eris
{

template<typename ProtocolT>
AsioStreamSocket<ProtocolT>::AsioStreamSocket(
        boost::asio::io_service& io_service, const std::string& client_name,
        Atlas::Bridge& bridge, StreamSocket::Callbacks& callbacks) :
        StreamSocket(io_service, client_name, bridge, callbacks), m_socket(io_service)
{
}

template<typename ProtocolT>
AsioStreamSocket<ProtocolT>::~AsioStreamSocket()
{
    try {
        m_socket.shutdown(ProtocolT::socket::shutdown_both);
    } catch (const std::exception& e) {
        warning() << "Error when shutting down socket.";
    }
    if (m_socket.is_open()) {
        try {
            m_socket.close();
        } catch (const std::exception& e) {
            warning() << "Error when closing socket.";
        }
    }
}


template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::connect(const typename ProtocolT::endpoint& endpoint)
{
    _connectTimer.expires_from_now(
            boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS));
    _connectTimer.async_wait([&](boost::system::error_code ec) {
        if (!ec) {
            _callbacks.stateChanged(CONNECTING_TIMEOUT);
        }
    });

    m_socket.async_connect(endpoint, [this](boost::system::error_code ec) {
        if (!ec) {
            this->_connectTimer.cancel();
            m_is_connected = true;
            this->startNegotiation();
        } else {
            _callbacks.stateChanged(CONNECTING_FAILED);
        }
    });
}


template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::negotiate_read()
{
    m_socket.async_read_some(mBuffer.prepare(read_buffer_size),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mBuffer.commit(length);
                    if (length > 0) {
                        auto negotiateResult = this->negotiate();
                        if (negotiateResult == Atlas::Negotiate::FAILED) {
                            m_socket.close();
                            _callbacks.stateChanged(NEGOTIATE_FAILED);
                            return;
                        }
                    }

                    //If the _sc instance is removed we're done with negotiation and should start the main loop.
                    if (_sc == nullptr) {
                        this->write();
                        this->do_read();
                    } else {
                        this->write();
                        this->negotiate_read();
                    }
                } else {
                    if (ec != boost::asio::error::operation_aborted) {
                        _callbacks.stateChanged(CONNECTION_FAILED);
                    }
                }
            });
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::do_read()
{
    m_socket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mReadBuffer.commit(length);
                    this->m_ios.rdbuf(&mReadBuffer);
                    m_codec->poll();
                    this->m_ios.rdbuf(&mBuffer);
                    _callbacks.dispatch();
                    this->do_read();
                } else {
                    if (ec != boost::asio::error::operation_aborted) {
                        if (!this->m_socket.is_open()) {
                            _callbacks.stateChanged(CONNECTION_FAILED);
                        }
                    }
                }
            });
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::write()
{
    if (mBuffer.size() != 0) {
        async_write(m_socket, mBuffer.data(),
                [this](boost::system::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        mBuffer.consume(length);
                    } else {
                        if (ec != boost::asio::error::operation_aborted) {
                            if (!this->m_socket.is_open()) {
                                _callbacks.stateChanged(CONNECTION_FAILED);
                            }
                        }
                    }
                });
    }
}
}

#endif /* STREAMSOCKET_IMPL_H_ */

