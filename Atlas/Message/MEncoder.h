// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include <Atlas/EncoderBase.h>

#include <map>
#include <vector>
#include <string>

namespace Atlas { namespace Message {

class Element;

typedef std::map<std::string, Element> MapType;
typedef std::vector<Element> ListType;

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

    ~Encoder() override = default;

    /// Send a message (must be a map!) in stream state.
    void streamMessageElement(const MapType & obj);
    /// Send an object as a map item.
    void mapElementItem(const std::string&, const Element&);
    void mapElementMapItem(const std::string&, const MapType&);
    void mapElementListItem(const std::string&, const ListType&);
    /// Send an object as a list item.
    void listElementItem(const Element&);
    void listElementMapItem(const MapType&);
    void listElementListItem(const ListType&);
};

} } // namespace Atlas::Message

#endif
