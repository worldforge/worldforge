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
    virtual void streamMessage() { m_bridge->streamMessage(); }
    virtual void streamEnd() { m_bridge->streamEnd(); }
    
    virtual void mapMapItem(const std::string& name)
    { m_bridge->mapMapItem(name); }
    virtual void mapListItem(const std::string& name)
    { m_bridge->mapListItem(name); }
    virtual void mapIntItem(const std::string& name, long i)
    { m_bridge->mapIntItem(name, i); }
    virtual void mapFloatItem(const std::string& name, double d)
    { m_bridge->mapFloatItem(name, d); }
    virtual void mapStringItem(const std::string& name, const std::string& s)
    { m_bridge->mapStringItem(name, s); }
    virtual void mapEnd() { m_bridge->mapEnd(); }
    
    virtual void listMapItem() { m_bridge->listMapItem(); }
    virtual void listListItem() { m_bridge->listListItem(); }
    virtual void listIntItem(long i) { m_bridge->listIntItem(i); }
    virtual void listFloatItem(double d) { m_bridge->listFloatItem(d); }
    virtual void listStringItem(const std::string& s)
    { m_bridge->listStringItem(s); }
    virtual void listEnd() { m_bridge->listEnd(); }

protected:
    /// The bridge that requests are forwarded to.
    Atlas::Bridge* m_bridge;
};

} // namespace Atlas 

#endif // ATLAS_ENCODERBASE_H
