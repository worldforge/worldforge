// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_ENCODERBASE_H
#define ATLAS_ENCODERBASE_H

#include "Bridge.h"

namespace Atlas {

/** Base class forwarding to Atlas::Bridge as skeleton for other encoders

This class is useful when building your own encoders that work on top of
Bridge (hence taking one as an argument).

It forwards all requests to a protected Bridge b, hence you can inherit from
it and your encoder can be used as a Bridge (for instance, as argument to
another encoder) without you needing to implement all these forwarding
requests.

Of course your encoder does not have to inherit from EncoderBase - you could
offer a completely different interface from that of bridge.

@see Bridge
*/

class EncoderBase : public Atlas::Bridge {
public:

    /// You will need to implement this in subclasses
    EncoderBase(Atlas::Bridge* b) : b(b) { }

    virtual ~EncoderBase() { }
    
    virtual void streamBegin() { b->streamBegin(); }
    virtual void streamMessage(const Map& m) { b->streamMessage(m); }
    virtual void streamEnd() { b->streamEnd(); }
    
    virtual void mapItem(const std::string& name, const Bridge::Map& m)
    { b->mapItem(name, m); }
    virtual void mapItem(const std::string& name, const Bridge::List& l)
    { b->mapItem(name, l); }
    virtual void mapItem(const std::string& name, int i)
    { b->mapItem(name, i); }
    virtual void mapItem(const std::string& name, double d)
    { b->mapItem(name, d); }
    virtual void mapItem(const std::string& name, const std::string& s)
    { b->mapItem(name, s); }
    virtual void mapEnd() { b->mapEnd(); }
    
    virtual void listItem(const Bridge::Map& m) { b->listItem(m); }
    virtual void listItem(const Bridge::List& l) { b->listItem(l); }
    virtual void listItem(int i) { b->listItem(i); }
    virtual void listItem(double d) { b->listItem(d); }
    virtual void listItem(const std::string& s) { b->listItem(s); }
    virtual void listEnd() { b->listEnd(); }

protected:
    /// The bridge that requests are forwarded to.
    Atlas::Bridge* b;
};

} // namespace Atlas 

#endif
