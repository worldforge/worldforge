// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_DECODERBASE_H
#define ATLAS_MESSAGE_DECODERBASE_H

#include <string>
#include <stack>
#include "../Bridge.h"
#include "Object.h"

namespace Atlas { namespace Message {

/** Base class for decoders that take Atlas::Message::Object.
 *
 * This class is passed to a codec as receiver bridge. It decodes a stream
 * into Message::Object objects, and after completion calls the abstract
 * objectArrived() method. This is to be overridden by base classes, which
 * might, for instance, provide an object queue or a callback method for
 * arrived messages.
 *
 * @see Atlas::Bridge
 * @see Atlas::Codec
 * @see Object
 * @author Stefanus Du Toit <sdt@gmx.net>
 * 
 */

class DecoderBase : public Bridge
{
public:
    DecoderBase();

    virtual ~DecoderBase() {}

    // Callback functions from Bridge
    virtual void streamBegin();
    virtual void streamMessage(const Map&);
    virtual void streamEnd();

    virtual void mapItem(const std::string& name, const Map&);
    virtual void mapItem(const std::string& name, const List&);
    virtual void mapItem(const std::string& name, int);
    virtual void mapItem(const std::string& name, double);
    virtual void mapItem(const std::string& name, const std::string&);
    virtual void mapEnd();
    
    virtual void listItem(const Map&);
    virtual void listItem(const List&);
    virtual void listItem(int);
    virtual void listItem(double);
    virtual void listItem(const std::string&);
    virtual void listEnd();
    
protected:

    /// Our current decoding state.
    enum State {
        STATE_STREAM,
        STATE_MAP,
        STATE_LIST
    };

    /// The state stack.
    std::stack<State> state;
    /// The map stack.
    std::stack<Object::MapType> maps;
    /// The list stack.
    std::stack<Object::ListType> lists;
    /// Names for maps and lists.
    std::stack<std::string> names;

    /// Override this - called when an object was received.
    virtual void objectArrived(const Object& obj) = 0;
};

} } // namespace Atlas::Message

#endif
