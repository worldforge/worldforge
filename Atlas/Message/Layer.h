// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_MESSAGE_LAYER_H
#define ATLAS_MESSAGE_LAYER_H

#include "../Bridge.h"
#include "Object.h"

namespace Atlas { namespace Message {

/** Atlas message layer

FIXME document this

XXX sdt --

some things I'm not sure about - this is meant to be symmetric, right, so
you use Layer for both a Bridge that acts as receiver and for a sender.

But if so, then that's a bit icky when it comes to the Object& item methods
being for sending but the other item methods for receiving..

What about layers that want to override the same methods for both retrieval and
sending?

Tell me whether I'm missing something somewhere :)

-- sdt XXX

@see Object
@see Bridge
*/

class Layer : public Atlas::Bridge
{
public:

    Layer(Atlas::Bridge*);

    virtual ~Layer() { }

    virtual void StreamMessage(const Object& obj);
    virtual void MapItem(const string& name, const Object& obj);
    virtual void ListItem(const Object& obj);

    virtual void ReceiveObject(Object& target);

protected:

    Object GetObject();
    
private:

    Atlas::Bridge* bridge;
    Object object;
    bool complete;
};

} } // Atlas::Message namespace

#endif
