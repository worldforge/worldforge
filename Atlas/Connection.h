// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_CONNECTION_H
#define ATLAS_CONNECTION_H

#include "Codec.h"

namespace Atlas {

/** Atlas connection

Connection encapsulates (or at the moment, fails to encapsulate) all the
information relating to an Atlas connection. At present this consists of a
codec, the stream that the codec is writing to and reading from and the name
of the peer on the other end of the connection.

@see Codec
@see Negotiate
*/

template <typename Stream>
struct Connection
{
    Codec<Stream>* codec;
    std::string peer;

    Connection(Codec<Stream>* codec, const std::string& peer)
	: codec(codec), peer(peer) { }
	
    Connection(const Connection& that)
	: codec(that.codec), peer(that.peer) { }
};

} // Atlas namespace

#endif
