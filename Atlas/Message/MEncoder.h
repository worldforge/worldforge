// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include "../EncoderBase.h"

namespace Atlas { namespace Message {

class Element;

/** Encoder that transmits Atlas::Message::Element.
 *
 * This encoder can be used to send Atlas::Message::Element objects representing
 * full atlas messages.
 *
 * @see Element
 * @see Atlas::Bridge
 * @see Atlas::EncoderBase
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Encoder : public Atlas::EncoderBase
{
public:
    Encoder(Atlas::Bridge*);

    virtual ~Encoder() { }

    /// Send a message (must be a map!) in stream state.
    virtual void streamMessage(const Element& obj);
    /// Send an object as a map item.
    virtual void mapItem(const std::string&, const Element&);
    /// Send an object as a list item.
    virtual void listItem(const Element&);

    /// Atlas::EncoderBase methods (so we don't hide them).
    virtual void streamMessage(const Map& m) {
        Atlas::EncoderBase::streamMessage(m);
    }

    virtual void mapItem(const std::string& name, const Bridge::Map& m) {
        Atlas::EncoderBase::mapItem(name, m);
    }
    virtual void mapItem(const std::string& name, const Bridge::List& l) {
        Atlas::EncoderBase::mapItem(name, l);
    }
    virtual void mapItem(const std::string& name, long i) {
        Atlas::EncoderBase::mapItem(name, i);
    }
    virtual void mapItem(const std::string& name, double d) {
        Atlas::EncoderBase::mapItem(name, d);
    }
    virtual void mapItem(const std::string& name, const std::string& s) {
        Atlas::EncoderBase::mapItem(name, s);
    }
    
    virtual void listItem(const Bridge::Map& m) {
        Atlas::EncoderBase::listItem(m);
    }
    virtual void listItem(const Bridge::List& l) {
        Atlas::EncoderBase::listItem(l);
    }
    virtual void listItem(long i) { Atlas::EncoderBase::listItem(i); }
    virtual void listItem(double d) { Atlas::EncoderBase::listItem(d); }
    virtual void listItem(const std::string& s) {
        Atlas::EncoderBase::listItem(s);
    }
};

} } // namespace Atlas::Message

#endif
