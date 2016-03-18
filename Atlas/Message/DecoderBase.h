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

    virtual ~DecoderBase();

    // Callback functions from Bridge
    virtual void streamBegin();
    virtual void streamMessage();
    virtual void streamEnd();

    virtual void mapMapItem(const std::string& name);
    virtual void mapListItem(const std::string& name);
    virtual void mapIntItem(const std::string& name, long);
    virtual void mapFloatItem(const std::string& name, double);
    virtual void mapStringItem(const std::string& name, const std::string&);
    virtual void mapEnd();
    
    virtual void listMapItem();
    virtual void listListItem();
    virtual void listIntItem(long);
    virtual void listFloatItem(double);
    virtual void listStringItem(const std::string&);
    virtual void listEnd();
    
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
