// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_NET_SOCKET_H
#define ATLAS_NET_SOCKET_H

#include <string>

namespace Atlas { namespace Net {

/** Socket interface for network data transmission

FIXME what's a socket? FIXME

*/

class Socket
{
    public:

    virtual ~Socket() { }

    /// Send data through socket
    virtual int	Send(const std::string&) = 0;

    /// Receive data from socket
    virtual int	Receive(std::string&) = 0;

    /// Shut down the connection
    virtual void Close() = 0;
};

} } // Atlas::Net

#endif
