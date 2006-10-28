// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

// Much inspiration, the original idea and name suggestion by Mike Day.

#ifndef ATLAS_FUNKY_ENCODER_H
#define ATLAS_FUNKY_ENCODER_H

#include <string>

namespace Atlas { namespace Funky {

/** @defgroup funky_encoder Atlas Funky Encoder
 *
 * A compile-time-semantics-checking-<<-style encoder.
 *
 * This encoder is composed of several classes which each have different
 * operator<<. You can use it to send a message through a bridge in a format
 * similar to the following:
 *
 * <PRE>
 * using namespace Funky;
 * Funky::Encoder enc(&myBridge);
 * enc << Token::begin_message
 *     << Token::begin_map
 *        << "an int" << 1234
 *        << "a float" << 3.142
 *        << "a string" << "Hello World!"
 *        << "a list" << Token::begin_list
 *           << 5678
 *           << 2.181
 *           << "another string!"
 *        << Token::end_list
 *     << Token::end_map
 *     << Token::end_message;
 * </PRE>
 * 
 * The special thing is that it will perform semantic checking automatically
 * <I>at compile time</I> via a template stack.
 *
 * @author Stefanus Du Toit <sdt@gmx.net>, with help of Mike Day <mikeday@corplink.com.au>
 * @see Atlas::Bridge
*/

/** Token class representing the beginning of a message.
 * 
 *  @ingroup funky_encoder
 * @see funky_encoder
 */
class BeginMessage {};
/** Token class representing the end of a message.
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
class EndMessage {};
/** Token class representing the beginning of a map.
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
class BeginMap {};
/** Token class representing the end of a map.
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
class EndMap {};
/** Token class representing the beginning of a list.
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
class BeginList {};
/** Token class representing the end of a list.
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
class EndList {};

template<class B> class FunkyEncoder;
template<class B, class T> class EncMap;
template<class B, class T> class EncList;
template<class B, class T> class EncMapValue;

/** Encoder in map value state
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
template<class B, class T>
class EncMapValue {
public:
    EncMapValue(B& b, const std::string& name) : b(b), name(name) { }
    
    /// Begin a map.
    EncMap<B, T> operator<<(const BeginMap&)
    {
        b.mapMapItem(name);
        return EncMap<B, T>(b);
    }

    /// Begin a list.
    EncList<B, T> operator<<(const BeginList&)
    {
        b.mapListItem(name);
        return EncList<B, T>(b);
    }

    /// Send an integer value.
    T operator<<(long i)
    {
        b.mapIntItem(name, i);
        return T(b);
    }

    /// Send a double value.
    T operator<<(double d)
    {
        b.mapFloatItem(name, d);
        return T(b);
    }

    /// Send a string value.
    T operator<<(const std::string& s)
    {
        b.mapStringItem(name, s);
        return T(b);
    }

    /// If the encoder supports it, send any kind of value.
    template<typename Arg>
    T operator<<(const Arg& a)
    {
        b.mapItem(name, a);
        return T(b);
    }

protected:
    /// The bridge or encoder that is written to.
    B& b;
    /// The name of this item
    std::string name;
};

/** Encoder in Map state
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
template<class B, class T>
class EncMap {
public:
    EncMap(B& b) : b(b) { }

    /// Start a value with its name
    EncMapValue< B, EncMap<B, T> > operator<<(const std::string& name)
    {
        return EncMapValue< B, EncMap<B, T> >(b, name);
    }

    /// End this map
    T operator<<(EndMap)
    {
        b.mapEnd();
        return T(b);
    }
    
protected:
    /// The bridge or encoder that is written to.
    B& b;
};

/** Encoder in List state
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
template<class B, class T>
class EncList {
public:
    EncList(B& b) : b(b) { }

    /// Start a map.
    EncMap<B, EncList<B, T> > operator<<(const BeginMap&)
    {
        b.listMapItem();
        return EncMap<B, EncList<B, T> >(b);
    }

    /// Start a list.
    EncList<B, EncList<B, T> > operator<<(const BeginList&)
    {
        b.listListItem();
        return EncList<B, EncList<B, T> >(b);
    }

    /// Send an integer value.
    EncList<B, T> operator<<(long i)
    {
        b.listIntItem(i);
        return *this;
    }

    /// Send a double value.
    EncList<B, T> operator<<(double d)
    {
        b.listFloatItem(d);
        return *this;
    }

    /// Send a string value.
    EncList<B, T> operator<<(const std::string& s)
    {
        b.listStringItem(s);
        return *this;
    }

    /// If the encoder supports it, send any kind of value.
    template<typename Arg>
    EncList<B, T> operator<<(const Arg& a)
    {
        b.listItem(a);
        return *this;
    }
    
    /// End this list.
    T operator<<(EndList)
    {
        b.listEnd();
        return T(b);
    }
    
protected:
    /// The bridge or encoder that is written to.
    B& b;
};

/** The root encoder in "stream" state.
 *
 * @ingroup funky_encoder
 * @see funky_encoder
 */
template <class B>
class FunkyEncoder
{
public:
    FunkyEncoder(B& b) : b(b) { }
    
    /// Start a message (as a map).
    EncMap<B, FunkyEncoder> operator<<(const BeginMap&) {
        b.streamMessage();
        return EncMap<B, FunkyEncoder>(b);
    }

    /// If the encoder supports it, send a different kind of message.
    template<typename Arg>
    FunkyEncoder<B> operator<<(const Arg& a)
    {
        b.streamObjectsMessage(a);
        return *this;
    }

protected:
    /// The bridge or encoder that is written to.
    B& b;
};

/** Tokens representing beginnings and ends of maps/lists.
 *
 * Use these as parameters to operator<< for the Funky::Encoder classes.
 * 
 * @see funky_encoder
 * @ingroup funky_encoder
 */
class Tokens {
public:
    static BeginMap begin_map;
    static EndMap end_map;
    static BeginList begin_list;
    static EndList end_list;
};


} } // Atlas::Funky namespace

#endif
