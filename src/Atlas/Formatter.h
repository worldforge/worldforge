// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

// $Id$

#ifndef ATLAS_FORMATTER_H
#define ATLAS_FORMATTER_H

#include <Atlas/Bridge.h>

namespace Atlas {

/** Atlas stream formatter

This class implements the Bridge interface that accepts an Atlas stream.
It copies the input stream to an output bridge, inserting formatting
characters according to settings defined at construction time.

@see Codec
*/

class Formatter : public Bridge
{
  public:
    
    Formatter(std::ostream & s, Atlas::Bridge & b);

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

    void setSpacing(int s) {
        m_spacing = s;
    }

  protected:

    std::ostream & m_stream;
    Bridge & m_bridge;
    int m_indent;
    int m_spacing;
};

} // Atlas namespace

#endif // ATLAS_FORMATTER_H
