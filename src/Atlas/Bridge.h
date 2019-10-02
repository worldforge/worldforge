// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#ifndef ATLAS_BRIDGE_H
#define ATLAS_BRIDGE_H

#include <string>

/** The Atlas namespace.

This namespace contains the whole of the Atlas-C++ library, and which is
divided into a hierarchy of other namespaces. The main namespaces of
interest to the application developers are the Atlas::Net namespace containing
classes to handle establishing network connections, and the Atlas::Objects
namespace containing classes used to handle high level Atlas data.
 */
namespace Atlas {

/** Atlas stream bridge

This class presents an interface that accepts an Atlas stream. The stream
begins with a call to streamBegin() and finishes with streamEnd(). While the
Bridge is in this stream context, a message can be sent using streamMessage().
This puts the Bridge into a map context, allowing various map*Item() calls.

Several classes are derived from Bridge, the most notable of which is Codec,
which accepts an Atlas stream for encoding and transmission.

@see Codec
*/

class Bridge
{
  public:
    virtual ~Bridge() = default;

    // Interface for stream context

    /**
     *  Begin an Atlas stream.
     */
    virtual void streamBegin() = 0;
    /**
     *  Start a message in an Atlas stream.
     */
    virtual void streamMessage() = 0;
    /**
     *  Ends the Atlas stream.
     */
    virtual void streamEnd() = 0;
    
    // Interface for map context

    /**
     *  Starts a map object to the currently streamed map.
     */
    virtual void mapMapItem(std::string name) = 0;
    /**
     *  Starts a list object to the currently streamed map.
     */
    virtual void mapListItem(std::string name) = 0;
    /**
     *  Adds an integer to the currently streames map.
     */
    virtual void mapIntItem(std::string name, long) = 0;
    /**
     *  Adds a float to the currently streamed map.
     */
    virtual void mapFloatItem(std::string name, double) = 0;
    /**
     *  Adds a string to the currently streamed map.
     */
    virtual void mapStringItem(std::string name, std::string) = 0;
    /**
     *  Ends the currently streamed map.
     */
    virtual void mapEnd() = 0;
    
    // Interface for list context
    
    /**
     *  Starts a map object in the currently streamed list.
     */
    virtual void listMapItem() = 0;
    /**
     *  Starts a list object in the currently streamed list.
     */
    virtual void listListItem() = 0;
    /**
     *  Adds an integer to the currently streames list.
     */
    virtual void listIntItem(long) = 0;
    /**
     *  Adds a float to the currently streamed list.
     */
    virtual void listFloatItem(double) = 0;
    /**
     *  Adds a string to the currently streamed list.
     */
    virtual void listStringItem(std::string) = 0;
    /**
     *  Ends the currently streamed list.
     */
    virtual void listEnd() = 0;
};

} // Atlas namespace

#endif // ATLAS_BRIDGE_H
