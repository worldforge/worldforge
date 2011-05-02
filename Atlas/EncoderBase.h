// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#ifndef ATLAS_ENCODERBASE_H
#define ATLAS_ENCODERBASE_H

#include <Atlas/Bridge.h>

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

class EncoderBase { // : public Atlas::Bridge {
public:

    /// You will need to implement this in subclasses
    EncoderBase(Atlas::Bridge & b) : m_b(b) { }

    virtual ~EncoderBase() { }
    
    void streamBegin() { m_b.streamBegin(); }
    void streamMessage() { m_b.streamMessage(); }
    void streamEnd() { m_b.streamEnd(); }
    
    void mapMapItem(const std::string& name)
    { m_b.mapMapItem(name); }
    void mapListItem(const std::string& name)
    { m_b.mapListItem(name); }
    void mapIntItem(const std::string& name, long i)
    { m_b.mapIntItem(name, i); }
    void mapFloatItem(const std::string& name, double d)
    { m_b.mapFloatItem(name, d); }
    void mapStringItem(const std::string& name, const std::string& s)
    { m_b.mapStringItem(name, s); }
    void mapEnd()
    { m_b.mapEnd(); }
    
    void listMapItem() { m_b.listMapItem(); }
    void listListItem() { m_b.listListItem(); }
    void listIntItem(long i) { m_b.listIntItem(i); }
    void listFloatItem(double d) { m_b.listFloatItem(d); }
    void listStringItem(const std::string& s) { m_b.listStringItem(s); }
    void listEnd() { m_b.listEnd(); }

protected:
    /// The bridge that requests are forwarded to.
    Atlas::Bridge & m_b;
};

} // namespace Atlas 

#endif // ATLAS_ENCODERBASE_H
