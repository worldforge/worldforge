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
    EncoderBase(Atlas::Bridge* b) : m_bridge(b) { }

    virtual ~EncoderBase() { }
    
    virtual void streamBegin() { m_bridge->streamBegin(); }
    virtual void streamMessage(const Map& m) { m_bridge->streamMessage(m); }
    virtual void streamEnd() { m_bridge->streamEnd(); }
    
    virtual void mapItem(const std::string& name, const Bridge::Map& m)
    { m_bridge->mapItem(name, m); }
    virtual void mapItem(const std::string& name, const Bridge::List& l)
    { m_bridge->mapItem(name, l); }
    virtual void mapItem(const std::string& name, long i)
    { m_bridge->mapItem(name, i); }
    virtual void mapItem(const std::string& name, double d)
    { m_bridge->mapItem(name, d); }
    virtual void mapItem(const std::string& name, const std::string& s)
    { m_bridge->mapItem(name, s); }
    virtual void mapEnd() { m_bridge->mapEnd(); }
    
    virtual void listItem(const Bridge::Map& m) { m_bridge->listItem(m); }
    virtual void listItem(const Bridge::List& l) { m_bridge->listItem(l); }
    virtual void listItem(long i) { m_bridge->listItem(i); }
    virtual void listItem(double d) { m_bridge->listItem(d); }
    virtual void listItem(const std::string& s) { m_bridge->listItem(s); }
    virtual void listEnd() { m_bridge->listEnd(); }

protected:
    /// The bridge that requests are forwarded to.
    Atlas::Bridge* m_bridge;
};

} // namespace Atlas 

#endif
