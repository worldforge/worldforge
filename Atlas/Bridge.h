// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_BRIDGE_H
#define ATLAS_BRIDGE_H

#include <string>

namespace Atlas {

/** Atlas stream bridge

This class presents an interface that accepts an Atlas stream. The stream
begins with a call to StreamBegin() and finishes with StreamEnd(). While the
Bridge is in this stream context, a message can be sent using StreamMessage().
This puts the Bridge into a map context, allowing various MapItem() calls.

Several classes are derived from Bridge, the most notable of which is Codec,
which accepts an Atlas stream for encoding and transmission.

@see Codec
*/

class Bridge
{
    public:
    
    class Map { } static MapBegin;
    class List { } static ListBegin;

    // Interface for stream context

    virtual void StreamBegin() = 0;
    virtual void StreamMessage(const Map&) = 0;
    virtual void StreamEnd() = 0;
    
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

} // Atlas namespace

#endif
