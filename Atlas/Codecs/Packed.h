// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Michael Day

// $Id$

#ifndef ATLAS_CODECS_PACKED_H
#define ATLAS_CODECS_PACKED_H

#include <Atlas/Codecs/Utility.h>
#include <Atlas/Codec.h>

#include <iosfwd>
#include <stack>

namespace Atlas { namespace Codecs {

/*

The form for each element of this codec is as follows:

[type][name=][data][|endtype]
  
( ) for lists
[ ] for maps
$ for string
@ for int
# for float

Sample output for this codec: (whitespace added for clarity)

[@id=17$name=Fred +28the +2b great+29#weight=1.5(args=@1@2@3)]

The complete specification is located in cvs at:
    forge/protocols/atlas/spec/packed_syntax.html
    
*/
  
class Packed : public Codec
{
public:
    
    Packed(std::istream& in, std::ostream& out, Atlas::Bridge & b);

    void poll(bool can_read) override;

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
    
    std::istream& m_istream;
    std::ostream& m_ostream;
    Bridge & m_bridge;

    enum State
    {
        PARSE_NOTHING,
        PARSE_STREAM,
        PARSE_MAP,
        PARSE_LIST,
	    PARSE_MAP_BEGIN,
	    PARSE_LIST_BEGIN,
        PARSE_INT,
        PARSE_FLOAT,
        PARSE_STRING,
        PARSE_NAME
    };
    
    std::stack<State> m_state;

    std::string m_name;
    std::string m_data;

    /**
     * Preallocated to increase performance.
     */
    std::string m_encoded;
    /**
     * Preallocated to increase performance.
     */
    std::string m_decoded;
    /**
     * Preallocated to increase performance.
     */
    char m_hex[3];

    void parsingBegins(char);
    void parseStream(char);
    void parseMap(char);
    void parseList(char);
    void parseMapBegin(char);
    void parseListBegin(char);
    void parseInt(char);
    void parseFloat(char);
    void parseString(char);
    void parseName(char);

    inline std::string hexEncode(std::string data)
    {

        for (size_t i = 0; i < data.size(); i++) {
            char currentChar = data[i];

            switch(currentChar) {
                case '+':
                case '[':
                case ']':
                case '(':
                case ')':
                case '@':
                case '#':
                case '$':
                case '=':
                    //First special character, use an encoded string instead
                    m_encoded.clear();
                    m_encoded.reserve(data.size() + (data.size() / 4));
                    m_encoded.assign(data, 0, i);
                    for (; i < data.size(); i++) {
                        currentChar = data[i];

                        switch(currentChar) {
                            case '+':
                            case '[':
                            case ']':
                            case '(':
                            case ')':
                            case '@':
                            case '#':
                            case '$':
                            case '=':
                                //First special character, use an encoded string instead
                                m_encoded += '+';
                                m_encoded += charToHex(currentChar);
                                break;
                            default:
                                m_encoded += currentChar;
                                break;
                        }
                    }

                    return std::move(m_encoded);
                default:
                    break;
            }
        }

        //If no special character, just return the original string, avoiding any allocations.
        return data;
    }

    inline const std::string hexDecode(std::string data)
    {

        for (size_t i = 0; i < data.size(); i++) {
            char currentChar = data[i];
            if (currentChar == '+') {
                //First special character, use a decoded string instead
                m_decoded.clear();
                m_decoded.reserve(data.size());
                m_decoded.assign(data, 0, i);

                for (; i < data.size(); i++) {
                    currentChar = data[i];
                    if (currentChar == '+') {
                        m_hex[0] = data[++i];
                        m_hex[1] = data[++i];
                        m_hex[2] = 0;
                        m_decoded += hexToChar(m_hex);
                    } else {
                        m_decoded += currentChar;
                    }
                }

                return std::move(m_decoded);
            }
        }

        //If no special character, just return the original string, avoiding any allocations.
        return data;
    }
};

} } // namespace Atlas::Codecs

#endif
