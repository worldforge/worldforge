// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_NEGOTIATE_H
#define ATLAS_NEGOTIATE_H

#include "Connection.h"

namespace Atlas {

/** Negotiation of codecs and filters for an Atlas connection

non blocking negotiation of Codecs and Filters
requires a list of avalable Codecs and Filters,
along with the name of sender and a Socket

@see Connection
@see Codec
@see Filter
*/

template <typename Stream>
class Negotiate
{
    public:

    enum State
    {
	IN_PROGRESS,
	SUCCEEDED,
	FAILED,
    };

    virtual State Poll() = 0;
    virtual Connection<Stream> GetConnection() = 0;
};

} // Atlas namespace

#endif
