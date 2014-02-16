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
        StreamSocket(io_service, client_name, bridge, callbacks), m_socket(
                io_service)
{
}

template<typename ProtocolT>
AsioStreamSocket<ProtocolT>::~AsioStreamSocket()
{
    if (m_socket.is_open()) {
        try {
            m_socket.shutdown(ProtocolT::socket::shutdown_both);
        } catch (const std::exception& e) {
            warning() << "Error when shutting down socket: " << e.what();
        }
        try {
            m_socket.close();
        } catch (const std::exception& e) {
            warning() << "Error when closing socket.";
        }
    }
}

template<typename ProtocolT>
ResolvableAsioStreamSocket<ProtocolT>::ResolvableAsioStreamSocket(
        boost::asio::io_service& io_service, const std::string& client_name,
        Atlas::Bridge& bridge, StreamSocket::Callbacks& callbacks) :
        AsioStreamSocket<ProtocolT>(io_service, client_name, bridge, callbacks),
        m_resolver(io_service)
{
}



template<typename ProtocolT>
void ResolvableAsioStreamSocket<ProtocolT>::connectWithQuery(
        const typename ProtocolT::resolver::query& query)
{
    auto self(this->shared_from_this());
    m_resolver.async_resolve(query,
            [&, self](const boost::system::error_code& ec, typename ProtocolT::resolver::iterator iterator) {
                if (this->_callbacks.stateChanged) {
                    if (!ec && iterator != typename ProtocolT::resolver::iterator()) {
                        this->connect(*iterator);
                    } else {
                        this->_callbacks.stateChanged(StreamSocket::CONNECTING_FAILED);
                    }
                }
            });
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::connect(
        const typename ProtocolT::endpoint& endpoint)
{
    _connectTimer.expires_from_now(
            boost::posix_time::seconds(CONNECT_TIMEOUT_SECONDS));
    auto self(this->shared_from_this());
    _connectTimer.async_wait([&, self](boost::system::error_code ec) {
        if (!ec) {
            if (_callbacks.stateChanged) {
                _callbacks.stateChanged(CONNECTING_TIMEOUT);
            }
        }
    });

    m_socket.async_connect(endpoint,
            [this, self](boost::system::error_code ec) {
                if (_callbacks.stateChanged) {
                    if (!ec) {
                        this->_connectTimer.cancel();
                        m_is_connected = true;
                        this->startNegotiation();
                    } else {
                        _callbacks.stateChanged(CONNECTING_FAILED);
                    }
                }
            });
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::negotiate_read()
{
    auto self(this->shared_from_this());
    m_socket.async_read_some(mBuffer->prepare(read_buffer_size),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (_callbacks.stateChanged) {
                    if (!ec)
                    {
                        mBuffer->commit(length);
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
                            this->negotiate_write();
                            this->negotiate_read();
                        }
                    } else {
                        if (ec != boost::asio::error::operation_aborted) {
                            _callbacks.stateChanged(CONNECTION_FAILED);
                        }
                    }
                }
            });
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::do_read()
{
    auto self(this->shared_from_this());
    m_socket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (_callbacks.stateChanged) {
                    if (!ec)
                    {
                        mReadBuffer.commit(length);
                        this->m_ios.rdbuf(&mReadBuffer);
                        m_codec->poll();
                        this->m_ios.rdbuf(mBuffer);
                        _callbacks.dispatch();
                        this->do_read();
                    } else {
                        if (ec != boost::asio::error::operation_aborted) {
                            if (!this->m_socket.is_open()) {
                                _callbacks.stateChanged(CONNECTION_FAILED);
                            }
                        }
                    }
                }
            });
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::write()
{

    if (mBuffer->size() != 0) {
        //When sending data we need to make sure that nothing else writes to the streambuf (mBuffer).
        //We do this by creating a new streambuf instance. The one containing the data to be sent is
        //then contained in a shared_ptr. When the async write operation is done the shared_ptr will
        //be deleted. However, in order to make this a bit more efficient we'll use a custom deleter
        //in which we'll check if the new buffer has had anything written to it. If not, we'll reuse
        //the buffer just used for writing, as this will already have had memory allocated.
        auto self(this->shared_from_this());
        std::function<void(boost::asio::streambuf*)> bufferDeleter =
                [&, self](boost::asio::streambuf* p) {
                    //Check if the existing writebuffer has had anything written to it.
                    if (this->mBuffer->size() > 0) {
                        delete p;
                    } else {
                        //If the existing writebuffer hasn't had anything written to it we'll reuse the previous
                        //one instead since it already have had memory allocated. This prevents unnecessary release
                        //and re-allocation of memory.
                        delete this->mBuffer;
                        this->mBuffer = p;
                        this->m_ios.rdbuf(this->mBuffer);

                    }
                };
        std::shared_ptr<boost::asio::streambuf> buffer(mBuffer, bufferDeleter);
        mBuffer = new boost::asio::streambuf();
        m_ios.rdbuf(mBuffer);

        async_write(m_socket, buffer->data(),
                [this, buffer, self](boost::system::error_code ec, std::size_t length)
                {
                    if (_callbacks.stateChanged) {
                        if (!ec)
                        {
                            buffer->consume(length);
                        } else {
                            if (ec != boost::asio::error::operation_aborted) {
                                if (!this->m_socket.is_open()) {
                                    _callbacks.stateChanged(CONNECTION_FAILED);
                                }
                            }
                        }
                    }
                });
    }
}

template<typename ProtocolT>
void AsioStreamSocket<ProtocolT>::negotiate_write()
{

    if (mBuffer->size() != 0) {
        auto self(this->shared_from_this());
        boost::asio::async_write(m_socket, mBuffer->data(),
                [this, self](boost::system::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        this->mBuffer->consume(length);
                    }
                });
    }
}

}

#endif /* STREAMSOCKET_IMPL_H_ */

