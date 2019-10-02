// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

// $Id$

#ifndef ATLAS_CODECS_BACH_H
#define ATLAS_CODECS_BACH_H

#include <Atlas/Codec.h>

#include <iosfwd>
#include <stack>

namespace Atlas {
    namespace Codecs {

/** @file Codecs/Bach.h
 * This class implements Bach Codec
 */

        class Bach : public Codec {
        public:

            Bach(std::istream &in, std::ostream &out, Atlas::Bridge &b);

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

            unsigned linenum() const { return m_linenum; }

        protected:

            std::istream &m_istream;
            std::ostream &m_ostream;
            Bridge &m_bridge;
            bool m_comma;
            unsigned m_linenum;

            enum State {
                PARSE_INIT,
                PARSE_STREAM,
                PARSE_MAP,
                PARSE_LIST,
                PARSE_NAME,
                PARSE_DATA,
                PARSE_INT,
                PARSE_FLOAT,
                PARSE_STRING,
                PARSE_LITERAL, // for literal character escaped with backslash
                PARSE_COMMENT // for when we're in the middle of a comment field
            };

            bool stringmode() const;

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

            inline void parseLiteral(char);

            inline void parseName(char);

            inline void parseComment(char);

            static inline std::string encodeString(std::string);

            static inline std::string decodeString(std::string);

            void writeIntItem(const std::string &, long);

            void writeFloatItem(const std::string &, double);

            void writeStringItem(const std::string &, std::string);

            void writeLine(const std::string &, bool= true, bool= false);
        };

    }
} // namespace Atlas::Codecs

#endif // ATLAS_CODECS_BACH_H
