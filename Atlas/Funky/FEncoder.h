// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_FUNKY_ENCODER_H
#define ATLAS_FUNKY_ENCODER_H

#include <utility>
#include <string>

#include "../Bridge.h"

namespace Atlas { namespace Funky {

/** Atlas funky encoder

@see Bridge
*/

class BeginMessage {};
class EndMessage {};
class BeginMap {};
class EndMap {};
class BeginList {};
class EndList {};

class Encoder;
template<class T> class EncMessage;
template<class T> class EncMap;
template<class T> class EncList;

template<class T>
class EncMap {
public:
    EncMap(Bridge& c) : c(c) { }

    EncMap< EncMap<T> > operator<<(
            const std::pair<std::string, BeginMap>& p)
    {
        c.MapItem(p.first, Bridge::MapBegin);
        return EncMap< EncMap<T> >(c);
    }

    EncList< EncMap<T> > operator<<(
            const std::pair<std::string, BeginList>& p)
    {
        c.MapItem(p.first, Bridge::ListBegin);
        return EncList< EncMap<T> >(c);
    }

    EncMap<T> operator<<(const std::pair<std::string, int>& p)
    {
        c.MapItem(p.first, p.second);
        return *this;
    }

    EncMap<T> operator<<(const std::pair<std::string, double>& p)
    {
        c.MapItem(p.first, p.second);
        return *this;
    }

    EncMap<T> operator<<(
            const std::pair<std::string, std::string>& p)
    {
        c.MapItem(p.first, p.second);
        return *this;
    }
    
    T operator<<(EndMap)
    {
        c.MapEnd();
        return T(c);
    }
    
protected:
    Bridge& c;
};

template<class T>
class EncList {
public:
    EncList(Bridge& c) : c(c) { }

    EncMap<EncList<T> > operator<<(const BeginMap&)
    {
        c.ListItem(Bridge::MapBegin);
        return EncMap<EncList<T> >(c);
    }

    EncList<EncList<T> > operator<<(const BeginList&)
    {
        c.ListItem(Bridge::ListBegin);
        return EncList<EncList<T> >(c);
    }

    EncList<T> operator<<(int i)
    {
        c.ListItem(i);
        return *this;
    }

    EncList<T> operator<<(double d)
    {
        c.ListItem(d);
        return *this;
    }

    EncList<T> operator<<(const std::string& s)
    {
        c.ListItem(s);
        return *this;
    }
    
    T operator<<(EndList)
    {
        c.ListEnd();
        return T(c);
    }
    
protected:
    Bridge& c;
};

template<class T>
class EncMessage
{
public:
    EncMessage(Bridge& c) : c(c) { }

    EncMap<EncMessage> operator<<(const BeginMap&)
    {
        c.MessageItem(Bridge::MapBegin);
        return EncMap<EncMessage>(c);
    }
        
    T operator<<(EndMessage)
    {
        c.MessageBegin();
        return T(&c);
    }
    
protected:
    Bridge& c;
};

class Encoder
{
public:
    Encoder(Bridge* b) : b(*b) { }
    
    EncMessage<Encoder> operator<<(const BeginMessage&) {
        b.MessageBegin();
        return EncMessage<Encoder>(b);
    }

    static BeginMessage begin_message;
    static EndMessage end_message;
    static BeginMap begin_map;
    static EndMap end_map;
    static BeginList begin_list;
    static EndList end_list;

protected:
    Bridge& b;
};

} } // Atlas::Funky namespace

#endif
