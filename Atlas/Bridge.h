// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_BRIDGE_H
#define ATLAS_BRIDGE_H

#include <string>

namespace Atlas {

/** Atlas stream bridge

This class presents an interface that accepts an Atlas stream. The stream
begins with a call to streamBegin() and finishes with streamEnd(). While the
Bridge is in this stream context, a message can be sent using streamMessage().
This puts the Bridge into a map context, allowing various mapItem() calls.

Several classes are derived from Bridge, the most notable of which is Codec,
which accepts an Atlas stream for encoding and transmission.

@see Codec
*/

class Bridge
{
    public:
    
    class Map { } static mapBegin;
    class List { } static listBegin;

    // Interface for stream context

    virtual void streamBegin() = 0;
    virtual void streamMessage(const Map&) = 0;
    virtual void streamEnd() = 0;
    
    // Interface for map context

    virtual void mapItem(const std::string& name, const Map&) = 0;
    virtual void mapItem(const std::string& name, const List&) = 0;
    virtual void mapItem(const std::string& name, int) = 0;
    virtual void mapItem(const std::string& name, double) = 0;
    virtual void mapItem(const std::string& name, const std::string&) = 0;
    virtual void mapEnd() = 0;
    
    // Interface for list context
    
    virtual void listItem(const Map&) = 0;
    virtual void listItem(const List&) = 0;
    virtual void listItem(int) = 0;
    virtual void listItem(double) = 0;
    virtual void listItem(const std::string&) = 0;
    virtual void listEnd() = 0;
};

} // Atlas namespace

#endif
