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

/** Atlas message decoder base
 *
 * This class is passed to a codec as receiver bridge. It decodes a stream
 * into Message::Object objects, and after completion calls the abstract
 * ObjectArrived() method. This is to be overridden by base classes, which
 * might, for instance, provide an object queue or a callback method for
 * arrived messages.
 *
 * @see Bridge
 * @see Codec
 * @see Object
 * 
 */

class DecoderBase : public Bridge
{
public:
    DecoderBase();

    virtual ~DecoderBase() {}

    // Callback functions from Bridge
    virtual void StreamBegin();
    virtual void StreamMessage(const Map&);
    virtual void StreamEnd();

    virtual void MapItem(const std::string& name, const Map&);
    virtual void MapItem(const std::string& name, const List&);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, double);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapEnd();
    
    virtual void ListItem(const Map&);
    virtual void ListItem(const List&);
    virtual void ListItem(int);
    virtual void ListItem(double);
    virtual void ListItem(const std::string&);
    virtual void ListEnd();
    
protected:

    enum State {
        STATE_STREAM,
        STATE_MAP,
        STATE_LIST
    };

    std::stack<State> state;
    std::stack<Object> objects;
    std::stack<string> names;

    // To be overridden by derived classes
    virtual void ObjectArrived(const Object& obj) = 0;
};

} } // namespace Atlas::Message

#endif
