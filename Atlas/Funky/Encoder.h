// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// Much inspiration, the original idea and name suggestion by Mike Day.

#ifndef ATLAS_FUNKY_ENCODER_H
#define ATLAS_FUNKY_ENCODER_H

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
template<class T> class EncMapValue;

template<class T>
class EncMapValue {
public:
    EncMapValue(Bridge& b, const string& name) : b(b), name(name) { }
    
    EncMap<T> operator<<(const BeginMap&)
    {
        b.MapItem(name, Bridge::MapBegin);
        return EncMap<T>(b);
    }

    EncList<T> operator<<(const BeginList&)
    {
        b.MapItem(name, Bridge::ListBegin);
        return EncList<T>(b);
    }

    T operator<<(int i)
    {
        b.MapItem(name, i);
        return T(b);
    }

    T operator<<(double d)
    {
        b.MapItem(name, d);
        return T(b);
    }

    T operator<<(const string& s)
    {
        b.MapItem(name, s);
        return T(b);
    }

protected:
    Bridge& b;
    string name;
};

template<class T>
class EncMap {
public:
    EncMap(Bridge& b) : b(b) { }

    EncMapValue< EncMap<T> > operator<<(const string& name)
    {
        return EncMapValue< EncMap<T> >(b, name);
    }

    T operator<<(EndMap)
    {
        b.MapEnd();
        return T(b);
    }
    
protected:
    Bridge& b;
};

template<class T>
class EncList {
public:
    EncList(Bridge& b) : b(b) { }

    EncMap<EncList<T> > operator<<(const BeginMap&)
    {
        b.ListItem(Bridge::MapBegin);
        return EncMap<EncList<T> >(b);
    }

    EncList<EncList<T> > operator<<(const BeginList&)
    {
        b.ListItem(Bridge::ListBegin);
        return EncList<EncList<T> >(b);
    }

    EncList<T> operator<<(int i)
    {
        b.ListItem(i);
        return *this;
    }

    EncList<T> operator<<(double d)
    {
        b.ListItem(d);
        return *this;
    }

    EncList<T> operator<<(const std::string& s)
    {
        b.ListItem(s);
        return *this;
    }
    
    T operator<<(EndList)
    {
        b.ListEnd();
        return T(b);
    }
    
protected:
    Bridge& b;
};

template<class T>
class EncMessage
{
public:
    EncMessage(Bridge& b) : b(b) { }

    EncMap<EncMessage> operator<<(const BeginMap&)
    {
        b.MessageItem(Bridge::MapBegin);
        return EncMap<EncMessage>(b);
    }
        
    T operator<<(EndMessage)
    {
        b.MessageEnd();
        return T(&b);
    }
    
protected:
    Bridge& b;
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
