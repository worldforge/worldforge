// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include "../EncoderBase.h"

#include "Object.h"

namespace Atlas { namespace Message {

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
    virtual void streamMessageElement(const Element::MapType & obj);
    /// Send an object as a map item.
    virtual void mapElementItem(const std::string&, const Element&);
    virtual void mapElementMapItem(const std::string&, const Element::MapType&);
    virtual void mapElementListItem(const std::string&, const Element::ListType&);
    /// Send an object as a list item.
    virtual void listElementItem(const Element&);
    virtual void listElementMapItem(const Element::MapType&);
    virtual void listElementListItem(const Element::ListType&);

    /// Atlas::EncoderBase methods (so we don't hide them).
    virtual void streamMessage() {
        Atlas::EncoderBase::streamMessage();
    }

    virtual void mapMapItem(const std::string& name) {
        Atlas::EncoderBase::mapMapItem(name);
    }
    virtual void mapListItem(const std::string& name) {
        Atlas::EncoderBase::mapListItem(name);
    }
    virtual void mapIntItem(const std::string& name, long i) {
        Atlas::EncoderBase::mapIntItem(name, i);
    }
    virtual void mapFloatItem(const std::string& name, double d) {
        Atlas::EncoderBase::mapFloatItem(name, d);
    }
    virtual void mapStringItem(const std::string& name, const std::string& s) {
        Atlas::EncoderBase::mapStringItem(name, s);
    }
    
    virtual void listMapItem() { Atlas::EncoderBase::listMapItem(); }
    virtual void listListItem() { Atlas::EncoderBase::listListItem(); }
    virtual void listIntItem(long i) { Atlas::EncoderBase::listIntItem(i); }
    virtual void listFloatItem(double d) {
        Atlas::EncoderBase::listFloatItem(d);
    }
    virtual void listStringItem(const std::string& s) {
        Atlas::EncoderBase::listStringItem(s);
    }
};

} } // namespace Atlas::Message

#endif
