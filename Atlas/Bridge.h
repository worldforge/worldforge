// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_BRIDGE_H
#define ATLAS_STREAM_BRIDGE_H

#include <string>

namespace Atlas { namespace Stream {

/** Atlas stream bridge

This class presents an interface for sending Atlas messages. Starting a new
message with MessageBegin() puts the encoder into a message context. A valid
message consists of one map from strings to values, created with
MessageBeginMap(). This puts the encoder into a map context, allowing named
attributes to be sent. These attributes include other nested maps and lists,
which put the encoder into nested map and list contexts respectively. When
the encoder is in a list context, it can send nameless values whose order is
significant.

Encoder is used by Codec to accept Atlas messages for conversion to a byte
stream and subsequent transmission.

@see Codec

*/

class Bridge
{
    public:
    
    class Map { } static MapBegin;
    class List { } static ListBegin;

    // Interface for message context

    virtual void MessageBegin() = 0;
    virtual void MessageItem(const Map&) = 0;
    virtual void MessageEnd() = 0;
    
    // Interface for map context

    virtual void MapItem(const std::string& name, const Map&) = 0;
    virtual void MapItem(const std::string& name, const List&) = 0;
    virtual void MapItem(const std::string& name, int) = 0;
    virtual void MapItem(const std::string& name, double) = 0;
    virtual void MapItem(const std::string& name, const std::string&) = 0;
    virtual void MapEnd() = 0;
    
    // Interface for list context
    
    virtual void ListItem(const Map&) = 0;
    virtual void ListItem(const List&) = 0;
    virtual void ListItem(int) = 0;
    virtual void ListItem(double) = 0;
    virtual void ListItem(const std::string&) = 0;
    virtual void ListEnd() = 0;
};

} } // Atlas::Stream

#endif
