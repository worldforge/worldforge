// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include <Atlas/Message/Element.h>

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
    explicit Encoder(Atlas::Bridge &);

    ~Encoder();

    /// Send a message (must be a map!) in stream state.
    void streamMessageElement(const Element::MapType & obj);
    /// Send an object as a map item.
    void mapElementItem(const std::string&, const Element&);
    void mapElementMapItem(const std::string&, const Element::MapType&);
    void mapElementListItem(const std::string&, const Element::ListType&);
    /// Send an object as a list item.
    void listElementItem(const Element&);
    void listElementMapItem(const Element::MapType&);
    void listElementListItem(const Element::ListType&);
};

} } // namespace Atlas::Message

#endif
