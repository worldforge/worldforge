// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_CONNECTION_H
#define ATLAS_STREAM_CONNECTION_H

#include "Codec.h"

namespace Atlas { namespace Stream {

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
    Stream& stream;
    std::string peer;

    Connection(Codec<Stream>* codec, Stream& stream, const std::string& peer)
	: codec(codec), stream(stream), peer(peer) { }
	
    Connection(const Connection& that)
	: codec(that.codec), stream(that.stream), peer(that.peer) { }
};

} } // Atlas::Stream

#endif
