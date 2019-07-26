// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

// $Id$

#ifndef ATLAS_CODECS_XML_H
#define ATLAS_CODECS_XML_H

#include <Atlas/Codec.h>

#include <iosfwd>
#include <stack>

namespace Atlas {
    namespace Codecs {

/*

Sample output for this codec: (whitespace added for clarity)

<atlas>
    <map>
	<int name="foo">13</int>
	<float name="meep">1.5</float>
	<string name="bar">hello</string>
	<list name="args">
	    <int>1</int>
	    <int>2</int>
	    <float>3.0</float>
	</list>
    </map>
</atlas>

The complete specification is located in cvs at:
    forge/protocols/atlas/spec/xml_syntax.html

*/

        class XML : public Codec {
        public:

            XML(std::istream &in, std::ostream &out, Atlas::Bridge &b);

            void poll() override;

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

            /**
             * Escapes a string for HTML.
             * @param The original string.
             * @return An escaped string.
             */
            static std::string escape(const std::string &);

            /**
             * Un-escapes a previously "escaped" string for HTML.
             * @param The original string.
             * @return An unescaped string.
             */
            static std::string unescape(const std::string &);

        protected:

            std::istream &m_istream;
            std::ostream &m_ostream;
            Bridge &m_bridge;

            enum Token {
                TOKEN_TAG,
                TOKEN_START_TAG,
                TOKEN_END_TAG,
                TOKEN_DATA
            };

            Token m_token;

            enum State {
                PARSE_NOTHING,
                PARSE_STREAM,
                PARSE_MAP,
                PARSE_LIST,
                PARSE_INT,
                PARSE_FLOAT,
                PARSE_STRING
            };

            std::stack<State> m_state;
            std::stack<std::string> m_data;

            std::string m_tag;
            std::string m_name;

            inline void tokenTag(char);

            inline void tokenStartTag(char);

            inline void tokenEndTag(char);

            inline void tokenData(char);

            inline void parseStartTag();

            inline void parseEndTag();

        };

    }
} // namespace Atlas::Codecs

#endif // ATLAS_CODECS_XML_H
