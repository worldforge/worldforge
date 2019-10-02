// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#ifndef ATLAS_NEGOTIATE_H
#define ATLAS_NEGOTIATE_H

namespace Atlas {

class Bridge;
class Codec;

/** Negotiation of codecs and filters for an Atlas connection

non blocking negotiation of Codecs and Filters
requires a list of avalable Codecs and Filters,
along with the name of sender and a Socket

@see Connection
@see Codec
@see Filter
*/

class Negotiate
{
    public:
    virtual ~Negotiate() = default;

    enum State
    {
        IN_PROGRESS,
        SUCCEEDED,
        FAILED
    };

    virtual State getState() = 0;
    virtual Codec * getCodec(Bridge &) = 0;
    virtual void poll() = 0;
};

} // Atlas namespace

#endif
