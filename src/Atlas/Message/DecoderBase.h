// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#ifndef ATLAS_MESSAGE_DECODERBASE_H
#define ATLAS_MESSAGE_DECODERBASE_H

#include <Atlas/Bridge.h>

#include <map>
#include <string>
#include <vector>
#include <stack>

namespace Atlas { namespace Message {

class Element;

typedef std::map<std::string, Element> MapType;
typedef std::vector<Element> ListType;


/** Base class for decoders that take Atlas::Message::Object.
 *
 * This class is passed to a codec as receiver bridge. It decodes a stream
 * into Message::Object objects, and after completion calls the abstract
 * messageArrived() method. This is to be overridden by base classes, which
 * might, for instance, provide an object queue or a callback method for
 * arrived messages.
 *
 * @see Atlas::Bridge
 * @see Atlas::Codec
 * @see Object
 * @author Stefanus Du Toit <sdt@gmx.net>
 * 
 */

class DecoderBase : public Bridge
{
public:
    DecoderBase();

    ~DecoderBase() override = default;

    // Callback functions from Bridge
    void streamBegin() override;
    void streamMessage() override;
    void streamEnd() override;

    void mapMapItem(std::string name) override;
    void mapListItem(std::string name) override;
    void mapIntItem(std::string name, long) override;
    void mapFloatItem(std::string name, double) override;
    void mapStringItem(std::string name, std::string) override;
    void mapEnd() override;
    
    void listMapItem() override;
    void listListItem() override;
    void listIntItem(long) override;
    void listFloatItem(double) override;
    void listStringItem(std::string) override;
    void listEnd() override;
    
protected:

    /// Our current decoding state.
    enum State {
        STATE_STREAM,
        STATE_MAP,
        STATE_LIST
    };

    /// The state stack.
    std::stack<State> m_state;
    /// The map stack.
    std::stack<MapType> m_maps;
    /// The list stack.
    std::stack<ListType> m_lists;
    /// Names for maps and lists.
    std::stack<std::string> m_names;

    /// Override this - called when an object was received.
    virtual void messageArrived(MapType obj) = 0;
};

} } // namespace Atlas::Message

#endif
