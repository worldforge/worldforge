// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

#ifndef ATLAS_CODECS_BACH_H
#define ATLAS_CODECS_BACH_H

#include <iostream>
#include <stack>

#include "../Codec.h"

namespace Atlas { namespace Codecs {

/** @file Codecs/Bach.h
 * This class implements Bach Codec
 */

class Bach : public Codec<std::iostream>
{
  public:

    Bach(std::iostream& s, Atlas::Bridge* b);

    virtual void poll(bool can_read = true);

    virtual void streamBegin();
    virtual void streamMessage(const Map&);
    virtual void streamEnd();

    virtual void mapItem(const std::string& name, const Map&);
    virtual void mapItem(const std::string& name, const List&);
    virtual void mapItem(const std::string& name, long);
    virtual void mapItem(const std::string& name, double);
    virtual void mapItem(const std::string& name, const std::string&);
    virtual void mapEnd();

    virtual void listItem(const Map&);
    virtual void listItem(const List&);
    virtual void listItem(double);
    virtual void listItem(long);
    virtual void listItem(const std::string&);
    virtual void listEnd();

  protected:

    std::iostream& m_socket;
    Bridge* m_bridge;
    bool m_comma, m_stringmode;
    int m_linenum;

    enum State
    {
        PARSE_INIT,
	PARSE_STREAM,
        PARSE_MAP,
        PARSE_LIST,
        PARSE_NAME,
        PARSE_DATA,
        PARSE_INT,
        PARSE_FLOAT,
        PARSE_STRING
    };

    std::string m_name, m_data;
    std::stack<State> m_state;

    inline void parseInit(char);
    inline void parseStream(char);
    inline void parseMap(char);
    inline void parseList(char);
    inline void parseData(char);
    inline void parseInt(char);
    inline void parseFloat(char);
    inline void parseString(char);
    inline void parseName(char);

    inline const std::string encodeString(std::string);
    inline const std::string decodeString(std::string);

    void writeItem(std::string,long);
    void writeItem(std::string,double);
    void writeItem(std::string,std::string);
    void writeLine(std::string,bool=true,bool=false);
};

} } // namespace Atlas::Codecs

#endif // ATLAS_CODECS_BACH_H
