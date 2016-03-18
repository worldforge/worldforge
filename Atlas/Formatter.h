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
