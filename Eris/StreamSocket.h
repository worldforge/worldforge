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

#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Negotiate.h>

#include <boost/asio.hpp>

namespace Atlas
{
    class Bridge;
    class Codec;
    namespace Net
    {
        class StreamConnect;
    }
    namespace Objects
    {
        class ObjectsEncoder;
    }
}


namespace Eris
{

class StreamSocket
{
    public:

    typedef enum {
        INVALID_STATUS = 0, ///< indicates an illegal state
        CONNECTING,     ///< stream / socket connection in progress
        CONNECTING_TIMEOUT,
        CONNECTING_FAILED,
        NEGOTIATE,      ///< Atlas negotiation in progress
        NEGOTIATE_TIMEOUT,
        NEGOTIATE_FAILED,
        CONNECTED,      ///< connection fully established
        CONNECTION_FAILED,
        DISCONNECTED,       ///< finished disconnection
        DISCONNECTING      ///< clean disconnection in progress
    } Status;

        struct Callbacks
        {
            std::function<void()> dispatch;
            std::function<void(Status)> stateChanged;
        };

        StreamSocket(boost::asio::io_service& io_service, const std::string& client_name, Atlas::Bridge& bridge, Callbacks& callbacks);
        virtual ~StreamSocket();

        std::iostream& getIos();

        Atlas::Codec& getCodec();
        Atlas::Objects::ObjectsEncoder& getEncoder();

        virtual void write() = 0;
    protected:
        enum
        {
            read_buffer_size = 16384
        };
        boost::asio::io_service& m_io_service;
        Atlas::Bridge& _bridge;
        Callbacks _callbacks;

        boost::asio::streambuf mBuffer;
        boost::asio::streambuf mReadBuffer;
        std::iostream m_ios;
        Atlas::Net::StreamConnect* _sc;     ///< negotiation object (NULL after connection!)
        boost::asio::deadline_timer _negotiateTimer;
        boost::asio::deadline_timer _connectTimer;
        Atlas::Codec* m_codec;
        Atlas::Objects::ObjectsEncoder * m_encoder;
        bool m_is_connected;

        virtual void do_read() = 0;
        virtual void negotiate_read() = 0;
        void startNegotiation();
        Atlas::Negotiate::State negotiate();

};


template<typename ProtocolT>
class AsioStreamSocket : public StreamSocket
{
    public:
        AsioStreamSocket(boost::asio::io_service& io_service, const std::string& client_name, Atlas::Bridge& bridge, StreamSocket::Callbacks& callbacks);
        virtual ~AsioStreamSocket();
        void connect(const typename ProtocolT::endpoint& endpoint);
        virtual void write();
   protected:
        typename ProtocolT::socket m_socket;
        virtual void negotiate_read();
        void negotiate_write();
        virtual void do_read();
};

}
#endif /* STREAMSOCKET_H_ */
