// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include <Atlas/Message/Object.h>

#include <Atlas/EncoderBase.h>

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
    explicit Encoder(Atlas::Bridge*);

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
};

} } // namespace Atlas::Message

#endif
