// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include "../EncoderBase.h"

namespace Atlas { namespace Message {

class Object;

/*%TODO(Jesse,Atlas,Encoder)
* There are two Encoder.h files and two Encoder.cpp files.
* This seems like a poor idea and, in fact, causes problems
* when building with CodeWarrior.
*/

/** Encoder that transmits Atlas::Message::Object.
 *
 * This encoder can be used to send Atlas::Message::Object objects representing
 * full atlas messages.
 *
 * @see Object
 * @see Atlas::Bridge
 * @see Atlas::EncoderBase
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Encoder : public Atlas::EncoderBase
{
    typedef Atlas::EncoderBase Inherited;
public:
    Encoder(Atlas::Bridge*);

    virtual ~Encoder() { }

    /// Send a message (must be a map!) in stream state.
    virtual void streamMessage(const Object& obj);
    /// Send an object as a map item.
    virtual void mapItem(const std::string&, const Object&);
    /// Send an object as a list item.
    virtual void listItem(const Object&);

    /// Inherited methods (so we don't hide them).
    virtual void streamMessage(const Map& m) {Inherited::streamMessage(m);}

    virtual void mapItem(const std::string& name, const Bridge::Map& m)
    {Inherited::mapItem(name, m);}
    virtual void mapItem(const std::string& name, const Bridge::List& l)
    {Inherited::mapItem(name, l);}
    virtual void mapItem(const std::string& name, long i)
    {Inherited::mapItem(name, i);}
    virtual void mapItem(const std::string& name, double d)
    {Inherited::mapItem(name, d);}
    virtual void mapItem(const std::string& name, const std::string& s)
    {Inherited::mapItem(name, s);}
    
    virtual void listItem(const Bridge::Map& m) {Inherited::listItem(m);}
    virtual void listItem(const Bridge::List& l) {Inherited::listItem(l);}
    virtual void listItem(long i) {Inherited::listItem(i);}
    virtual void listItem(double d) {Inherited::listItem(d);}
    virtual void listItem(const std::string& s) {Inherited::listItem(s);}
};

} } // namespace Atlas::Message

#endif
